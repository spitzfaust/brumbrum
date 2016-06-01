//////////////////////
// OBES IPC EXERCISE //
//      BY          //
// THOMAS RAUHOFER  //
//     AND          //
//  TOBIAS WATZEK   //
//////////////////////
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include "autobahn.h"
#include "rlutil.h"

/**
 * Ugly globals but thats the only way that signalhandling can do its work:
 */
/** Name of the program */
char* prog_name = NULL;
/** fifo to read from */
int display = -1;

/** Cleanup at the end */
void cleanup();
/** handle a SIGINT */
void handle_sigint(int sig);
/** print the help */
void print_help();



int main(int argc, char* argv[]) {
  // save the default terminal colors
  saveDefaultColor();
  // register the sigint handler
  (void)signal(SIGINT, handle_sigint);
  // puffer to read from fifo
  char incoming[PIPE_BUF] = "";
  char c;
  // malloc memory for the program name
  prog_name = (char*)malloc((strlen(argv[0]) + 1) * sizeof(char));
  // copy the program name from argv into the var
  strcpy(prog_name, argv[0]);
  // get options
  while ((c = getopt(argc, argv, "h")) != -1) {
    switch (c) {
      case 'h':
        // help option
        print_help(argv[0]);
        cleanup();
        return EXIT_SUCCESS;
        break;
      default:
        // no valid option
        setBackgroundColor(RED);
        fprintf(stderr, BG_RED "Error: %s No correct input was made.", argv[0]);
        clear_eol();
        cleanup();
        resetColor();
        return EXIT_FAILURE;
    }
  }
  // try to open the fifo
  display = open(PIPE_DISPLAY, O_RDONLY);
  /** Fifo was not successfully opened */
  if (display == -1) {
    // print error msg and exit
    setBackgroundColor(RED);
    fprintf(stderr, BG_RED "Error %s: Can't open fifo. Is the gridserver running?", prog_name);
    clear_eol();
    cleanup();
    resetColor();
    return EXIT_FAILURE;
  }
  // print pretty welcome msg
  setColor(LIGHTBLUE);
  printf("\nWelcome to the griddisplay\n");
  printf("\\ō͡≡o˞̶\n");
  printf("by Thomas Rauhofer\nand Tobias Watzek\n\n");
  resetColor();
  /** continuously read from fifo */
  while (1) {
    // get the read status
    int status = read(display, incoming, PIPE_BUF);
    /** could read from fifo */
    if (status > 0) {
      // print to stdout
      printf("%s", incoming);
    }
    /** could not read from fifo */
    else if (status == -1) {
      // print error msg and exit
      setBackgroundColor(RED);
      fprintf(stderr, BG_RED "Error %s: Can't read from the fifo", prog_name);
      clear_eol();
      cleanup();
      resetColor();
      return EXIT_FAILURE;
    }
  }
  cleanup();
  return EXIT_SUCCESS;
}

/**
 * Free memory and close the fifo
 */
void cleanup() {
  clear_eol();
  printf("Info %s: Exiting...", prog_name);
  clear_eol();
  printf("Info %s: Closing the fifo", prog_name);
  clear_eol();
  close(display);
  printf("Info %s: Freeing memory", prog_name);
  clear_eol();
  free(prog_name);
}

/**
 * Properly handle STRG+C/SIGINT
 * @param sig signal that was catched
 */
void handle_sigint(int sig) {
  // Make everything pretty
  printf("\n");
  setBackgroundColor(GREEN);
  // Call the cleanup function
  cleanup();
  resetColor();
  // exit the program
  exit(sig);
}

/**
 * Print a usage Message to the screen
 */
void print_help() {
  setBackgroundColor(BLUE);
  printf("\u2502Usage %s:", prog_name);
  clear_eol();
  printf("\u2502 First start the gridserver.");
  clear_eol();
  printf("\u2502 After that you can start the griddisplay.");
  clear_eol();
  resetColor();
}
