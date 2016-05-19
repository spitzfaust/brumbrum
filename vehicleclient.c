#include <stdbool.h>
#include <stdio.h>
#include <signal.h>
#include <ctype.h>
#include "rlutil.h"
#include "autobahn.h"

void handle_sig(int sig)
{
   printf("CTRL+C (Sig Nr:%d)not ignored...\n",sig);
   exit(sig);
}


int main(int argc, char const* argv[]) {
  char client_id = '0';
  char command = '0';
  bool cont = true;
  int msgid = -1;

  (void) signal(SIGINT, handle_sig);

  /* Argument Handling */
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <ID CHAR>\n", argv[0]);
    return EXIT_FAILURE;
  }
  client_id = argv[1][0];
  if(!isupper(client_id)) {
    fprintf(stderr, "Error: %s You have to use an upper case letter as ID\n", argv[0]);
    return EXIT_FAILURE;return EXIT_FAILURE;
  }
  // Message Queue oeffnen
  /*if ((msgid = msgget(KEY, PERM)) == -1) {
    // error handling
    fprintf(stderr, "%s: Can't access message queue\n", argv[0]);
    return EXIT_FAILURE;
  }*/
  /*
    Nachricht verschicken
    msg.mType = 1;
    strncpy(msg.mText, argv[1], MAX_DATA);
    if (msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0) == -1) {
       error handling
      fprintf(stderr, "%s: Can't send message\n", argv[0]);
      return EXIT_FAILURE;
    }*/
  saveDefaultColor();
  setColor(LIGHTBLUE);
  printf("\nWelcome to the Vehicleclient\n");
  printf("\\ō͡≡o˞̶\n");
  printf("by Thomas Rauhofer\nand Tobias Watzek\n\n");
  printf("Your ID is %c\n\n", client_id);
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
    switch (command) {
      case 'N':
        printf("North\n");
        break;
      case 'E':
        printf("East\n");
        break;
      case 'S':
        printf("South\n");
        break;
      case 'W':
        printf("West\n");
        break;
      case 'T':
        printf("Term\n");
        cont = false;
        break;
      default:
        printf("Command not found.\n");
        break;
    }
  }
  // printf("Message sent: %s\n", msg.mText);
  return EXIT_SUCCESS;
}
