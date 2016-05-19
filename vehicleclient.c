#include <stdbool.h>
#include <stdio.h>
#include <signal.h>

#include "rlutil.h"
#include "autobahn.h"

void myhandler(int sig)
{
   printf("CTRL+C (Sig Nr:%d)not ignored...\n",sig);
   return EXIT_SUCCESS;
}


int main(int argc, char const* argv[]) {
  char client_id = '0';
  char command = '0';
  bool cont = true;
  int msgid = -1;

  (void) signal(SIGINT,myhandler);

  /* Argument Handling */
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <CHAR>\n", argv[0]);
    return EXIT_FAILURE;
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
  printf("\nVehicleclient\n");
  printf("by Thomas Rauhofer\nand Tobias Watzek\n\n");
  resetColor();
  while (cont) {
    setColor(LIGHTMAGENTA);
    printf("Commands:\n");
    resetColor();
    printf("<N> North\n");
    printf("<E> East\n");
    printf("<S> South\n");
    printf("<W> West\n");
    setColor(LIGHTRED);
    printf("<T> Terminate\n");
    resetColor();
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
