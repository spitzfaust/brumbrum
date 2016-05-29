#include <stdbool.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include "rlutil.h"
#include "autobahn.h"

/**
 * Ugly globals but thats the only way that signalhandling can do its work:
 */
char* prog_name = NULL;
char client_id = 0;
int msgid = -1;

void handle_sigint(int sig);
void handle_sigterm(int sig);
char* dirIDtoStr(char id);

int main(int argc, char const* argv[]) {
  (void)signal(SIGINT, handle_sigint);
  (void)signal(SIGTERM, handle_sigterm);
  char command = '0';
  bool cont = true;
  long my_pid = (long)getpid();
  navigation msg;
  position init_pos;
  prog_name = (char*)malloc((strlen(argv[0]) + 1) * sizeof(char));
  strcpy(prog_name, argv[0]);

  /* Argument Handling */
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <ID CHAR>\n", prog_name);
    return EXIT_FAILURE;
  }
  client_id = toupper(argv[1][0]);
  if (!isalpha(client_id)) {
    fprintf(stderr, "Error: %s You have to use a letter as ID\n", prog_name);
    return EXIT_FAILURE;
  }
  if (!isupper(client_id)) {
    fprintf(stderr, "Error: %s You have to use an upper case letter as ID\n",
            prog_name);
    return EXIT_FAILURE;
  }
  msg.msg_to = SERVER;
  msg.msg_from = my_pid;
  msg.client_id = client_id;
  msg.direction = 'i';
  /* Message Queue oeffnen von msgget */
  if ((msgid = msgget(KEY, PERM)) == -1) {
    /* error handling */
    fprintf(stderr, "%s: Can't access message queue\n", prog_name);
    return EXIT_FAILURE;
  }
  if (msgsnd(msgid, &msg, sizeof(msg), 0) == -1) {
    /* error handling */
    fprintf(stderr, "%s: Can't send discovery message\n", prog_name);
    return EXIT_FAILURE;
  }
  if (msgrcv(msgid, &init_pos, sizeof(init_pos), my_pid, 0) == -1) {
    // error handling
    fprintf(stderr, "%s: Can't receive initial position from message queue\n",
            prog_name);
    return EXIT_FAILURE;
  }
  if (init_pos.status == REG_FULL) {
    fprintf(stderr, "%s: The grid is full.\n", prog_name);
    return EXIT_FAILURE;
  }
  if (init_pos.status == REG_DOUBLE) {
    fprintf(stderr, "%s: The ID %c is allready in use.\n", prog_name,
            client_id);
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
    command = toupper(command);
    if (command == 'N' || command == 'E' || command == 'S' || command == 'W' ||
        command == 'T') {
      msg.direction = command;
      if (msgsnd(msgid, &msg, sizeof(msg), 0) == -1) {
        /* error handling */
        fprintf(stderr, "%s: Can't send command\n", prog_name);
        return EXIT_FAILURE;
      }
      if (command != 'T') {
        printf("%c navigated %s\n", msg.client_id, dirIDtoStr(msg.direction));
      }
    } else {
      printf("Command not found.\n");
    }
  }
  return EXIT_SUCCESS;
}

void handle_sigint(int sig) {
  if (prog_name != NULL) {
    setColor(LIGHTBLUE);
    printf("\nInfo: %s SIGINT received\n", prog_name);
    resetColor();
    if(msgid != -1) {
      setColor(LIGHTBLUE);
      printf("Info: %s Sending terminate command to server. \n", prog_name);
      resetColor();
      navigation msg;
      msg.msg_to = SERVER;
      msg.msg_from = (long)getpid();
      msg.client_id = client_id;
      msg.direction = 'T';
      if (msgsnd(msgid, &msg, sizeof(msg), 0) == -1) {
        /* error handling */
        fprintf(stderr, "%s: Can't send command\n", prog_name);
        exit(EXIT_FAILURE);
      }
    } else {
      exit(sig);
    }
  }
}

void handle_sigterm(int sig) {
  if (prog_name != NULL) {
    printf("\nInfo: %s SIGTERM received\n", prog_name);
    printf("\nInfo: %s Vehicle %c has been eliminated.\n", prog_name, client_id);
    exit(sig);
  } else {
    printf("\nVehicle has been eliminated.\n");
    exit(sig);
  }
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
