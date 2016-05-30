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
char* prog_name = NULL;
int display = -1;

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

void handle_sigint(int sig) {
  printf("\n");
  setBackgroundColor(GREEN);
  cleanup();
  resetColor();
  exit(sig);
}

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

int main(int argc, char* argv[]) {
  saveDefaultColor();
  (void)signal(SIGINT, handle_sigint);
  char incoming[PIPE_BUF] = "";
  char c;
  prog_name = (char*)malloc((strlen(argv[0]) + 1) * sizeof(char));
  strcpy(prog_name, argv[0]);
  while ((c = getopt(argc, argv, "h")) != -1) {
    switch (c) {
      case 'h':
        print_help(argv[0]);
        cleanup();
        return EXIT_SUCCESS;
        break;
      default:
        setBackgroundColor(RED);
        fprintf(stderr, BG_RED "Error: %s No correct input was made.", argv[0]);
        clear_eol();
        cleanup();
        resetColor();
        return EXIT_FAILURE;
    }
  }

  display = open(PIPE_DISPLAY, O_RDONLY);
  if (display == -1) {
    setBackgroundColor(RED);
    fprintf(stderr, BG_RED "Error %s: Can't open fifo. Is the gridserver running?",
            prog_name);
    clear_eol();
    cleanup();
    resetColor();
    return EXIT_FAILURE;
  }

  setColor(LIGHTBLUE);
  printf("\nWelcome to the griddisplay\n");
  printf("\\ō͡≡o˞̶\n");
  printf("by Thomas Rauhofer\nand Tobias Watzek\n\n");
  resetColor();
  while (1) {
    int status = read(display, incoming, PIPE_BUF);
    if (status > 0) {
      printf("%s", incoming);
    } else if(status == -1){
      setBackgroundColor(RED);
      fprintf(stderr, BG_RED "Error %s: Can't read from the fifo",
              prog_name);
      clear_eol();
      cleanup();
      resetColor();
      return EXIT_FAILURE;
    }
  }
  cleanup();
  return EXIT_SUCCESS;
}
