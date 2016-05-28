#include <stdbool.h>
#include <stdio.h>
#include <signal.h>
#include <ctype.h>
#include "rlutil.h"
#include "autobahn.h"

void handle_sig(int sig) {
  printf("CTRL+C (Sig Nr:%d)not ignored...\n", sig);
  exit(sig);
}

char* dirIDtoStr(char id) {
  switch (id) {
    case 'N':
      return "north";
      break;
    case 'E':
      return "east";
      break;
    case 'S':
      return "south";
      break;
    case 'W':
      return "west";
      break;
    default:
      return "somewhere";
      break;
  }
}

int main(int argc, char const* argv[]) {
  char client_id = '0';
  char command = '0';
  bool cont = true;
  int msgid = -1;
  long my_pid = (long)getpid();
  navigation msg;
  position init_pos;

  (void)signal(SIGINT, handle_sig);

  /* Argument Handling */
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <ID CHAR>\n", argv[0]);
    return EXIT_FAILURE;
  }
  client_id = toupper(argv[1][0]);
  if (!isalpha(client_id)) {
    fprintf(stderr, "Error: %s You have to use a letter as ID\n",
            argv[0]);
    return EXIT_FAILURE;
  }
  if (!isupper(client_id)) {
    fprintf(stderr, "Error: %s You have to use an upper case letter as ID\n",
            argv[0]);
    return EXIT_FAILURE;
  }
  msg.msg_to = SERVER;
  msg.msg_from = my_pid;
  msg.client_id = client_id;
  msg.direction = 'i';
  /* Message Queue oeffnen von msgget */
  if ((msgid = msgget(KEY, PERM)) == -1) {
    /* error handling */
    fprintf(stderr, "%s: Can't access message queue\n", argv[0]);
    return EXIT_FAILURE;
  }
  if (msgsnd(msgid, &msg, sizeof(msg), 0) == -1) {
    /* error handling */
    fprintf(stderr, "%s: Can't send discovery message\n", argv[0]);
    return EXIT_FAILURE;
  }
  if (msgrcv(msgid, &init_pos, sizeof(init_pos), my_pid, 0) == -1) {
    // error handling
    fprintf(stderr, "%s: Can't receive initial position from message queue\n", argv[0]);
    return EXIT_FAILURE;
  }
  saveDefaultColor();
  setColor(LIGHTBLUE);
  printf("\nWelcome to the Vehicleclient\n");
  printf("\\ō͡≡o˞̶\n");
  printf("by Thomas Rauhofer\nand Tobias Watzek\n\n");
  printf("Your ID is %c\n\n", client_id);
  printf("Your vehicle is parked at:\n");
  printf("x: %d\n", init_pos.x);
  printf("y: %d\n", init_pos.y);
  resetColor();
  while (cont) {
    printf("\u256D");
    setColor(LIGHTMAGENTA);
    printf(" Commands:\n");
    resetColor();
    printf("\u251C N North\n");
    printf("\u251C E East\n");
    printf("\u251C S South\n");
    printf("\u251C W West\n");
    printf("\u251C T Terminate\n");
    printf("\u2570\u25B6 ");
    scanf(" %c", &command);
    if (command == 'N' || command == 'E' || command == 'S' || command == 'W' ||
        command == 'T') {
      msg.direction = command;
      if (msgsnd(msgid, &msg, sizeof(msg), 0) == -1) {
        /* error handling */
        fprintf(stderr, "%s: Can't send command\n", argv[0]);
        return EXIT_FAILURE;
      }
      if (command == 'T') {
        cont = false;
        printf("Terminated the vehicle %c.\n", msg.client_id);
      } else {
        printf("%c navigated %s\n", msg.client_id, dirIDtoStr(msg.direction));
      }
    } else {
      printf("Command not found.\n");
    }
  }
  return EXIT_SUCCESS;
}
