//////////////////////
// OBES IPC EXERCISE //
//      BY          //
// THOMAS RAUHOFER  //
//     AND          //
//  TOBIAS WATZEK   //
//////////////////////
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "autobahn.h"

/**
 * Ugly globals but thats the only way that signalhandling can do its work:
 */
/** grid for the vehicles */
char* grid = NULL;
/** name of the program */
char* prog_name = NULL;
/** msg queue */
int msgid = -1;
/** client pids */
long clients[26] = {};
/** fifo for display */
int display = -1;

/** handling signals */
void sig_handler();
/** cleanup on exit */
void cleanup();
/** check if client_id is on the board */
bool on_board(char id, long clients[]);
/* convert directio into position on the grid */
int dir_check(char dir, int width);
/** check if move is possible */
int move(char id, char dir, char grid[], int width, int size);

int main(int argc, char* argv[]) {
  /** register various signal handlers */
  (void)signal(SIGINT, sig_handler);
  (void)signal(SIGQUIT, sig_handler);
  (void)signal(SIGTERM, sig_handler);
  (void)signal(SIGHUP, sig_handler);
  /** Size of the grid */
  int size = 0;
  /** height of the grid */
  int height = 0;
  /** width of the grid */
  int width = 0;
  /** buffer to print to the display(fifo) */
  char output[PIPE_BUF] = "";
  /** received navigation msg */
  navigation msg;
  /** malloc memory for the program name */
  prog_name = (char*)malloc((strlen(argv[0]) + 1) * sizeof(char));
  /** copy the program name from the  arguments to the var */
  strcpy(prog_name, argv[0]);
  /** Check if enough arguments where submitted */
  if (argc != 5) {
    /** no size was specified -> set the height and width to 10 */
    height = 10;
    width = 10;
    /** print a warning msg to inform about correct input */
    setBackgroundColor(BROWN);
    printf("Warning: %s No correct input was made. The grid size was chosen at 10 x 10", prog_name);
    clear_eol();
    printf("Correct inputlooks like this: './gridserver -x 10 -y 10'");
    clear_eol();
    resetColor();
  } else {
    /** get x an y options */
    char c;
    while ((c = getopt(argc, argv, "x:y:")) != -1)
      switch (c) {
        case 'x':
          width = atoi(optarg);
          break;
        case 'y':
          height = atoi(optarg);
          break;
        default:
          setBackgroundColor(RED);
          fprintf(stderr, BG_RED "Error: %s No correct input was made.", prog_name);
          cleanup();
          resetColor();
          return EXIT_FAILURE;
      }
  }
  /** sizes are invalid */
  if (width <= 0 || height <= 0) {
    /** print error msg and exit */
    setBackgroundColor(RED);
    fprintf(stderr, BG_RED "Error %s: No correct input was made. Use positive dimensions.",
            prog_name);
    cleanup();
    resetColor();
    return EXIT_FAILURE;
  }
  /** define the size of the 1d representation of the grid */
  size = (width + 2) * (height + 2);
  /** malloc memory for the grid */
  grid = (char*)malloc((size) * sizeof(char));

  /** Fill the board and draw a boarder around */
  for (int i = 0; i < size; ++i) {
    if (i < width + 2 || i > (size - (width + 2))) {
      grid[i] = '#';
    } else if ((i % (width + 2) == (width + 1)) || (i % (width + 2) == 0)) {
      grid[i] = '#';
    } else {
      grid[i] = ' ';
    }
  }

  struct stat st;
  /** check if the fifo doesn't allready exists */
  if (stat(PIPE_DISPLAY, &st) != 0) {
    /** create the fifo */
    if (mkfifo(PIPE_DISPLAY, PERM) == -1) {
      setBackgroundColor(RED);
      fprintf(stderr, BG_RED "Error %s: Can't create fifo.", prog_name);
      clear_eol();
      cleanup();
      resetColor();
      return EXIT_FAILURE;
    }
  }
  /** open the fifo */
  display = open(PIPE_DISPLAY, O_RDWR);
  /** can't open fifo */
  if (display == -1) {
    /** print error msg and exit */
    setBackgroundColor(RED);
    fprintf(stderr, BG_RED "Error %s: Can't open fifo.", prog_name);
    clear_eol();
    cleanup();
    resetColor();
    return EXIT_FAILURE;
  }
  /** create msg queue and ceck if error is encountered */
  if ((msgid = msgget(KEY, PERM | IPC_CREAT | IPC_EXCL)) == -1) {
    /** print error msg and exit */
    setBackgroundColor(RED);
    fprintf(stderr, BG_RED "Error %s: Can't create message queue", prog_name);
    clear_eol();
    cleanup();
    resetColor();
    return EXIT_FAILURE;
  }
  /** welcome msg */
  setColor(LIGHTBLUE);
  printf("\nWelcome to the gridserver\n");
  printf("\\ō͡≡o˞̶\n");
  printf("by Thomas Rauhofer\nand Tobias Watzek\n\n");
  resetColor();
  /** get msges in a loop */
  while (1) {
    /** try to receive msg and check if an error is encountered */
    if (msgrcv(msgid, &msg, sizeof(msg), SERVER, 0) == -1) {
      /** print error msg and exit */
      setBackgroundColor(RED);
      fprintf(stderr, BG_RED "Error %s: Can't receive from message queue", prog_name);
      clear_eol();
      cleanup();
      resetColor();
      return EXIT_FAILURE;
    }

    printf("Message received: Client ID: %c Client command: %c\n", msg.client_id, msg.command);
    /** new vehicle shall be created */
    if (msg.command == 'i') {
      printf("New\n");
      /** initial position to send back to the client */
      position init_pos;
      /** receiver of the msg (client pid) */
      init_pos.msg_to = msg.msg_from;
      /** sender of the msg (server id) */
      init_pos.msg_from = SERVER;
      /** id of the client */
      init_pos.client_id = msg.client_id;
      /** set pos to 0 */
      init_pos.x = 0;
      init_pos.y = 0;
      /** set the status to REG_DOUBLE if the client id is allready in use else 0 */
      init_pos.status = (on_board(msg.client_id, clients) ? REG_DOUBLE : 0);
      /** if the client id is not allready in use */
      if (init_pos.status != REG_DOUBLE) {
        /** search for an empty spot on the grid */
        for (int i = 0; i < size; ++i) {
          if (grid[i] == ' ') {
            /** place the client on the grid */
            grid[i] = msg.client_id;
            /** add the pid of the client to the clients array */
            clients[msg.client_id - 'A'] = (long)msg.msg_from;
            /** calculate the x and y positions on the grid */
            init_pos.x = i % (width + 2) - 1;
            init_pos.y = i / (width + 2) - 1;
            /** set the status to REG_OK */
            init_pos.status = REG_OK;
            printf("%c registration ok\n", msg.client_id);
            break;
          }
        }
        /** status is not REG_OK -> grid must be full */
        if (init_pos.status == 0) {
          init_pos.status = REG_FULL;
        }
      }
      /** try to send the pos to the client and check if errors where encountered */
      if (msgsnd(msgid, &init_pos, sizeof(init_pos), 0) == -1) {
        /** print error msg and exit */
        setBackgroundColor(RED);
        fprintf(stderr, BG_RED "Error %s: Can't send position back to client", prog_name);
        clear_eol();
        cleanup();
        resetColor();
        exit(EXIT_FAILURE);
      }
    }
    /** client is on the board and want's to terminate */
    else if (on_board(msg.client_id, clients) && msg.command == 'T') {
      /** find the client on the grid */
      for (int i = 0; i < size; ++i) {
        if (grid[i] == msg.client_id) {
          printf("%c was terminated by User\n", msg.client_id);
          /** send SIGTERM to the pid of the client */
          kill(clients[msg.client_id - 'A'], SIGTERM);
          /** remove the client from the arrays */
          clients[msg.client_id - 'A'] = 0;
          grid[i] = ' ';
        }
      }
    }
    /** client is on board */
    else if (on_board(msg.client_id, clients)) {
      printf("Already on grid\n");
      /* move with crash */
      if (move(msg.client_id, msg.command, grid, width, size) == 0) {
        /* Client is found on grid */
        for (int i = 0; i < size; ++i) {
          if (grid[i] == msg.client_id) {
            printf("A crash occoured! %c and %c where destroyed!\n", grid[i],
                   grid[i + dir_check(msg.command, width)]);
            kill(clients[grid[i] - 'A'], SIGTERM);
            clients[grid[i] - 'A'] = 0;
            kill(clients[grid[i + dir_check(msg.command, width)] - 'A'], SIGTERM);
            clients[grid[i + dir_check(msg.command, width)] - 'A'] = 0;
            grid[i] = ' ';
            grid[i + dir_check(msg.command, width)] = ' ';
            break;
          }
        }
        /* move without crash but out of the grid */
      } else if (move(msg.client_id, msg.command, grid, width, size) == 2) {
        /* Client is found on grid */
        for (int i = 0; i < size; ++i) {
          if (grid[i] == msg.client_id) {
            printf("%c moved of board and was destroyed!\n", msg.client_id);
            kill(clients[grid[i] - 'A'], SIGTERM);
            clients[grid[i] - 'A'] = 0;
            grid[i] = ' ';
            break;
          }
        }
      }
      /** move without crash  */
      else if (move(msg.client_id, msg.command, grid, width, size) == 1) {
        /* Client is found on grid */
        for (int i = 0; i < size; ++i) {
          if (grid[i] == msg.client_id) {
            printf("%c moved in the direction of %c \n", msg.client_id, msg.command);
            grid[i + dir_check(msg.command, width)] = msg.client_id;
            grid[i] = ' ';
            break;
          }
        }
      }
    }
    /** store the board to the output buffer */
    /* print the board */
    printf("-------------------------------\n");
    output[0] = '\n';
    int size_count = 1;
    for (int y = 0; y < height + 2; ++y) {
      for (int x = 0; x < width + 2; ++x) {
        output[size_count] = grid[y * (width + 2) + x];
        ++size_count;
      }
      output[size_count] = '\n';
      ++size_count;
    }
    output[size_count] = '\n';
    output[size_count + 1] = '\0';
    /** try to write the grid to the display(fifo) and check if errors are encountered */
    if (write(display, output, strlen(output)) == -1) {
      /** print error msg and exit */
      setBackgroundColor(RED);
      fprintf(stderr, BG_RED "Error %s: Can't write to display\n", prog_name);
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
 * cleanup on exit
 * remove msg queue
 * kill all clients (send sigterm)
 * close the fifo
 * remove the fifo
 * free memory
 */
void cleanup() {
  clear_eol();
  printf("Info %s: Exiting...", prog_name);
  clear_eol();
  if (msgid != -1) {
    printf("Info %s: Cleaning up the message queue...", prog_name);
    clear_eol();
    msgctl(msgid, IPC_RMID, (struct msqid_ds*)0);
  }
  printf("Info %s: Killing Clients...", prog_name);
  clear_eol();
  for (int i = 0; i < 26; ++i) {
    if (clients[i] != 0) {
      kill(clients[i], SIGTERM);
    }
  }
  printf("Info %s: Closing the fifo...", prog_name);
  clear_eol();
  close(display);
  remove(PIPE_DISPLAY);
  printf("Info %s: Freeing memory...", prog_name);
  clear_eol();
  free(grid);
  free(prog_name);
}
/**
 * Handle a signal
 */
void sig_handler() {
  printf("\n");
  setBackgroundColor(GREEN);
  cleanup();
  resetColor();
  exit(EXIT_SUCCESS);
}
/**
 * Check if the client id is on the board
 * @param  id      client_id
 * @param  clients list of possible clients
 * @return         true if client is on the board else false
 */
bool on_board(char id, long clients[]) {
  /** pid is saved so client exists on the board */
  if (clients[id - 'A'] != 0) {
    return true;
  }
  return false;
}

/**
 * calculate factor to add to the index to get into a direction
 * @param  dir   direction to move in
 * @param  width width of the grid
 * @return       factor to add
 */
int dir_check(char dir, int width) {
  if (dir == 'N') {
    return (-(width + 2));
  } else if (dir == 'E') {
    return 1;
  } else if (dir == 'S') {
    return (width + 2);
  } else if (dir == 'W') {
    return -1;
  } else {
    return 0;
  }
}
/**
 * Check if move is possible
 * @param  id    client id
 * @param  dir   direction to move
 * @param  grid  grid with vehicles
 * @param  width width of the grid
 * @param  size  size of the grid array
 * @return       returns 1 if move is possible without crash, 2 if move is of grid and 0 if move
 * results in crash
 */
int move(char id, char dir, char grid[], int width, int size) {
  for (int i = 0; i < size; ++i) {
    if (grid[i] == id) {
      if (grid[i + dir_check(dir, width)] == '#') {
        return 2;
      }
      if (grid[i + dir_check(dir, width)] == ' ') {
        return 1;
      } else {
        return 0;
      }
    }
  }
  return 3;
}
