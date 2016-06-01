//////////////////////
// OBES IPC EXERCISE //
//      BY          //
// THOMAS RAUHOFER  //
//     AND          //
//  TOBIAS WATZEK   //
//////////////////////
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
// name of the program
char* prog_name = NULL;
// id of the client
char client_id = 0;
// id of the msg queue
int msgid = -1;

/** handle sigint */
void handle_sigint(int sig);
/** handle sigterm */
void handle_sigterm(int sig);
/** cleanup on exit */
void cleanup();
/** direction to string */
char* dirIDtoStr(char id);

int main(int argc, char const* argv[]) {
  // register signalhandler for SIGINT
  (void)signal(SIGINT, handle_sigint);
  // register signalhandler for SIGTERM
  (void)signal(SIGTERM, handle_sigterm);
  /** command that is send to the server */
  char command = '0';
  /** pid of the vehicleclient */
  long my_pid = (long)getpid();
  /** msg that is send to the server */
  navigation msg;
  /** initial position of the vehicleclient on the grid */
  position init_pos;
  // malloc memory for the program name
  prog_name = (char*)malloc((strlen(argv[0]) + 1) * sizeof(char));
  // copy the program name from argv into the var
  strcpy(prog_name, argv[0]);

  /** Not enough arguments where given */
  if (argc != 2) {
    // print error/usage msg and exit
    setBackgroundColor(RED);
    fprintf(stderr, BG_RED "Usage: %s <ID CHAR>", prog_name);
    clear_eol();
    cleanup();
    resetColor();
    return EXIT_FAILURE;
  }
  /** get the client id from the given arguments and try to convert it to an uppercase letter */
  client_id = toupper(argv[1][0]);
  /** The given id is not a letter */
  if (!isalpha(client_id)) {
    // print error msg and exit
    setBackgroundColor(RED);
    fprintf(stderr, BG_RED "Error: %s You have to use a letter as ID", prog_name);
    clear_eol();
    cleanup();
    resetColor();
    return EXIT_FAILURE;
  }
  /**
   * The given id is not uppercase
   * (should not happen because of the above conversion but it doesn't hurt, so let's check it)
   */
  if (!isupper(client_id)) {
    // print error msg and exit
    setBackgroundColor(RED);
    fprintf(stderr, BG_RED "Error: %s You have to use an upper case letter as ID", prog_name);
    clear_eol();
    cleanup();
    resetColor();
    return EXIT_FAILURE;
  }
  /** setup the discovery msg to the server */
  /** receiver of the msg */
  msg.msg_to = SERVER;
  /** sender of the msg */
  msg.msg_from = my_pid;
  /** client id */
  msg.client_id = client_id;
  /** command to send. i = initial */
  msg.command = 'i';
  /** try to get the msgid */
  msgid = msgget(KEY, PERM);
  /** error encountered - couldn't get msgid */
  if (msgid == -1) {
    /* print error msg and exit */
    setBackgroundColor(RED);
    fprintf(stderr, BG_RED "Error %s: Can't access message queue", prog_name);
    clear_eol();
    cleanup();
    resetColor();
    return EXIT_FAILURE;
  }
  /** try to send the discovery msg and check if an error was encountered */
  if (msgsnd(msgid, &msg, sizeof(msg), 0) == -1) {
    /* print error msg and exit*/
    setBackgroundColor(RED);
    fprintf(stderr, BG_RED "Error %s: Can't send discovery message", prog_name);
    return EXIT_FAILURE;
  }
  /** try to receive the response message with the inital position from the server and check if an
   * error was encountered  */
  if (msgrcv(msgid, &init_pos, sizeof(init_pos), my_pid, 0) == -1) {
    /** print error msg and exit */
    setBackgroundColor(RED);
    fprintf(stderr, BG_RED "Error %s: Can't receive initial position from message queue",
            prog_name);
    clear_eol();
    cleanup();
    resetColor();
    return EXIT_FAILURE;
  }
  /** check if the grid is full */
  if (init_pos.status == REG_FULL) {
    /** print error msg and exit */
    setBackgroundColor(RED);
    fprintf(stderr, BG_RED "Error %s: The grid is full.", prog_name);
    clear_eol();
    cleanup();
    resetColor();
    return EXIT_FAILURE;
  }
  /** check if the id is allready in use */
  else if (init_pos.status == REG_DOUBLE) {
    /** print error msg and exit */
    setBackgroundColor(RED);
    fprintf(stderr, BG_RED "%s: The ID %c is allready in use.", prog_name, client_id);
    clear_eol();
    cleanup();
    resetColor();
    return EXIT_FAILURE;
  }
  /** SETUP DONE  -  LET'S START */
  saveDefaultColor();
  /** Welcome msg */
  setColor(LIGHTBLUE);
  printf("\nWelcome to the vehicleclient\n");
  printf("\\ō͡≡o˞̶\n");
  printf("by Thomas Rauhofer\nand Tobias Watzek\n\n");
  printf("Your ID is %c\n\n", client_id);
  printf("Your vehicle is parked at:\n");
  printf("x: %d\n", init_pos.x);
  printf("y: %d\n", init_pos.y);
  resetColor();
  /** Get commands from the user until T is entered */
  while (true) {
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
    /** get user selection */
    scanf(" %c", &command);
    /** convert selection to uppercase */
    command = toupper(command);
    /** check if command is valid */
    if (command == 'N' || command == 'E' || command == 'S' || command == 'W' || command == 'T') {
      /** set the msg command to the new command */
      msg.command = command;
      /** try to send the msg and check if an error was encountered */
      if (msgsnd(msgid, &msg, sizeof(msg), 0) == -1) {
        /** print error msg and exit */
        setBackgroundColor(RED);
        fprintf(stderr, BG_RED "Error %s: Can't send command", prog_name);
        clear_eol();
        cleanup();
        resetColor();
        return EXIT_FAILURE;
      }
      /** check if no terminate command was entered */
      if (command != 'T') {
        /** print a pretty navigation msg */
        printf("%c navigated %s\n", msg.client_id, dirIDtoStr(msg.command));
      }
    } else {
      /** print error msg that command was not found */
      setColor(YELLOW);
      printf("Command not found.\n");
      resetColor();
    }
  }
  cleanup();
  return EXIT_SUCCESS;
}

/**
 * Free memory
 */
void cleanup() {
  clear_eol();
  printf("Info %s: Exiting...", prog_name);
  clear_eol();
  printf("Info %s: Freeing memory", prog_name);
  clear_eol();
  free(prog_name);
}

/**
 * Handle STRG+C/SIGINT
 * @param sig signal to handle
 */
void handle_sigint(int sig) {
  setBackgroundColor(BLUE);
  printf("\nInfo %s: SIGINT received", prog_name);
  clear_eol();
  /** check if msg queue is available */
  if (msgid != -1) {
    /** Send a terminate msg to the server */
    printf("Info %s: Sending terminate command to server", prog_name);
    clear_eol();
    resetColor();
    /** create a navigation msg */
    navigation msg;
    msg.msg_to = SERVER;
    msg.msg_from = (long)getpid();
    msg.client_id = client_id;
    msg.command = 'T';
    /** Check if the msg could be sent */
    if (msgsnd(msgid, &msg, sizeof(msg), 0) == -1) {
      /** print error msg and exit */
      setBackgroundColor(RED);
      fprintf(stderr, BG_RED "Error %s: Can't send command", prog_name);
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
/**
 * Handle a SIGTERM
 * @param sig signal to handle
 */
void handle_sigterm(int sig) {
  resetColor();
  setBackgroundColor(GREEN);
  printf("\nInfo %s: SIGTERM received", prog_name);
  clear_eol();
  printf("Info %s: Vehicle %c has been eliminated.", prog_name, client_id);
  clear_eol();
  cleanup();
  resetColor();
  exit(sig);
}

/**
 * Convert a dir char command to a string
 * @param  id direction
 * @return    direction as string
 */
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
