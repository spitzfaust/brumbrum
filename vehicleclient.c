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
void cleanup();
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
    setBackgroundColor(RED);
    fprintf(stderr, BG_RED"Usage: %s <ID CHAR>", prog_name);
    clear_eol();
    cleanup();
    resetColor();
    return EXIT_FAILURE;
  }
  client_id = toupper(argv[1][0]);
  if (!isalpha(client_id)) {
    setBackgroundColor(RED);
    fprintf(stderr, BG_RED"Error: %s You have to use a letter as ID", prog_name);
    clear_eol();
    cleanup();
    resetColor();
    return EXIT_FAILURE;
  }
  if (!isupper(client_id)) {
    setBackgroundColor(RED);
    fprintf(stderr, BG_RED"Error: %s You have to use an upper case letter as ID",
            prog_name);
    clear_eol();
    cleanup();
    resetColor();
    return EXIT_FAILURE;
  }
  msg.msg_to = SERVER;
  msg.msg_from = my_pid;
  msg.client_id = client_id;
  msg.command = 'i';
  /* Message Queue oeffnen von msgget */
  if ((msgid = msgget(KEY, PERM)) == -1) {
    /* error handling */
    setBackgroundColor(RED);
    fprintf(stderr, BG_RED "Error %s: Can't access message queue", prog_name);
    clear_eol();
    cleanup();
    resetColor();
    return EXIT_FAILURE;
  }
  if (msgsnd(msgid, &msg, sizeof(msg), 0) == -1) {
    /* error handling */
    setBackgroundColor(RED);
    fprintf(stderr, BG_RED "Error %s: Can't send discovery message", prog_name);
    return EXIT_FAILURE;
  }
  if (msgrcv(msgid, &init_pos, sizeof(init_pos), my_pid, 0) == -1) {
    // error handling
    setBackgroundColor(RED);
    fprintf(stderr, BG_RED "Error %s: Can't receive initial position from message queue",
            prog_name);
    clear_eol();
    cleanup();
    resetColor();
    return EXIT_FAILURE;
  }
  if (init_pos.status == REG_FULL) {
    setBackgroundColor(RED);
    fprintf(stderr, BG_RED "Error %s: The grid is full.", prog_name);
    clear_eol();
    cleanup();
    resetColor();
    return EXIT_FAILURE;
  } else if (init_pos.status == REG_DOUBLE) {
    setBackgroundColor(RED);
    fprintf(stderr, BG_RED "%s: The ID %c is allready in use.", prog_name,
            client_id);
    clear_eol();
    cleanup();
    resetColor();
    return EXIT_FAILURE;
  }
  saveDefaultColor();
  setColor(LIGHTBLUE);
  printf("\nWelcome to the vehicleclient\n");
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
      msg.command = command;
      if (msgsnd(msgid, &msg, sizeof(msg), 0) == -1) {
        /* error handling */
        setBackgroundColor(RED);
        fprintf(stderr, BG_RED "Error %s: Can't send command", prog_name);
        clear_eol();
        cleanup();
        resetColor();
        return EXIT_FAILURE;
      }
      if (command != 'T') {
        printf("%c navigated %s\n", msg.client_id, dirIDtoStr(msg.command));
      } else {
        cont = false;
      }
    } else {
      setColor(YELLOW);
      printf("Command not found.\n");
      resetColor();
    }
  }
  return EXIT_SUCCESS;
}

void cleanup() {
  clear_eol();
  printf("Info %s: Exiting...", prog_name);
  clear_eol();
  printf("Info %s: Freeing memory", prog_name);
  clear_eol();
  free(prog_name);
}

void handle_sigint(int sig) {
  setBackgroundColor(BLUE);
  printf("\nInfo %s: SIGINT received", prog_name);
  clear_eol();
  if(msgid != -1) {
    printf("Info %s: Sending terminate command to server", prog_name);
    clear_eol();
    resetColor();
    navigation msg;
    msg.msg_to = SERVER;
    msg.msg_from = (long)getpid();
    msg.client_id = client_id;
    msg.command = 'T';
    if (msgsnd(msgid, &msg, sizeof(msg), 0) == -1) {
      /* error handling */
      setBackgroundColor(RED);
      fprintf(stderr, BG_RED"Error %s: Can't send command", prog_name);
      clear_eol();
      resetColor();
      exit(EXIT_FAILURE);
    }
  } else {
    cleanup();
    resetColor();
    exit(sig);
  }
}

void handle_sigterm(int sig) {
  resetColor();
  setBackgroundColor(GREEN);
  printf("\nInfo %s: SIGTERM received", prog_name);
  clear_eol();
  printf("Info %s: Vehicle %c has been eliminated.", prog_name, client_id);
  clear_eol();
  cleanup();
  resetColor();
  exit(EXIT_SUCCESS);
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
