#include <string.h>
#include <stdbool.h>
#include "autobahn.h"
#include "rlutil.h"

/**
 * Ugly globals but thats the only way that signalhandling can do its work:
 */
char* grid = NULL;
char * prog_name = NULL;
int msgid = -1;

void sig_handler();
bool on_board(char id, long clients[]);
/* returns value for movin in 1D array */
int dir_check(char dir, char grid[], int width);
/* returns 1 if move is possible without crash, 2 if move is of grid
   and 0 if move results in crash */
int move(char id, char dir, char grid[], int width, int size);

int main(int argc, char* argv[]) {
  struct sigaction my_action;
  my_action.sa_handler = sig_handler;
  my_action.sa_flags = SA_RESTART;
  sigemptyset(&my_action.sa_mask);
  sigaction (SIGSEGV, &my_action, NULL);
  sigaction (SIGBUS, &my_action, NULL);
  sigaction (SIGABRT, &my_action, NULL);
  sigaction (SIGPIPE, &my_action, NULL);
  sigaction (SIGINT, &my_action, NULL);
  sigaction (SIGILL, &my_action, NULL);
  sigaction (SIGTERM, &my_action, NULL);
  sigaction (SIGHUP, &my_action, NULL);
  int size = 0;
  int height = 0;
  int width = 0;
  long clients[26] = {};
  /* Buffer fuer navigation am grid */
  navigation msg;
  prog_name = (char*)malloc((strlen(argv[0]) + 1) * sizeof(char));
  strcpy(prog_name, argv[0]);


  if (argc != 5) {
    height = 10;
    width = 10;
    printf(
        "Warning: %s No correct input was made. The grid size was chosen at 10 "
        "x 10\nCorrect inputlooks like this: \"./gridserver -x 10 -y 10\"\n",
        prog_name);
  } else {
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
          fprintf(stderr, "Error: %s No correct input was made.", prog_name);
          return EXIT_FAILURE;
      }
  }
  size = (width + 2) * (height + 2);
  grid = (char*)malloc((size) * sizeof(char));
  /* the board is written */
  for (int i = 0; i < size; ++i) {
    if (i < width + 2 || i > (size - (width + 2))) {
      grid[i] = '#';
    } else if ((i % (width + 2) == (width + 1)) || (i % (width + 2) == 0)) {
      grid[i] = '#';
    } else {
      grid[i] = ' ';
    }
  }



  /* Message Queue neu anlegen */
  if ((msgid = msgget(KEY, PERM | IPC_CREAT | IPC_EXCL)) == -1) {
    // error handling
    fprintf(stderr, "%s: Error creating message queue\n", prog_name);
    return EXIT_FAILURE;
  }

  /* In einer Endlosschleife Nachrichten empfangen */
  while (1) {
    if (msgrcv(msgid, &msg, sizeof(msg), SERVER, 0) == -1) {
      // error handling
      fprintf(stderr, "%s: Can't receive from message queue\n", prog_name);
      return EXIT_FAILURE;
    }

    printf("Message received: Client ID: %c Client direction: %c\n",
           msg.client_id, msg.direction);

    if (msg.direction == 'i') {
      fflush(stdout);
      printf("New\n");
      position init_pos;
      init_pos.msg_to = msg.msg_from;
      init_pos.msg_from = SERVER;
      init_pos.client_id = msg.client_id;
      init_pos.x = 0;
      init_pos.y = 0;
      init_pos.status = (on_board(msg.client_id, clients) ? REG_DOUBLE : 0);
      if (init_pos.status != REG_DOUBLE) {
        for (int i = 0; i < size; ++i) {
          if (grid[i] == ' ') {
            grid[i] = msg.client_id;
            clients[msg.client_id - 'A'] = (long)msg.msg_from;
            init_pos.x = i % width;
            init_pos.y = i / width;
            init_pos.status = REG_OK;
            break;
          }
        }
      }
      if (msgsnd(msgid, &init_pos, sizeof(init_pos), 0) == -1) {
        fprintf(stderr, "%s: Can't send position back to client\n", prog_name);
        exit(-1);
      }
    } else if (on_board(msg.client_id, clients) && msg.direction == 'T') {
      for (int i = 0; i < size; ++i) {
        if (grid[i] == msg.client_id) {
          printf("%c was terminated by User\n", msg.client_id);
          clients[msg.client_id - 'A'] = 0;
          grid[i] = ' ';
          grid[i + dir_check(msg.direction, grid, width)] = ' ';
        }
      }
    } else if (on_board(msg.client_id, clients)) {
      printf("Already on grid\n");
      /* move with crash */
      if (move(msg.client_id, msg.direction, grid, width, size) == 0) {
        /* Client is found on grid */
        for (int i = 0; i < size; ++i) {
          if (grid[i] == msg.client_id) {
            printf("A crash occoured! %c and %c where destroyed!\n", grid[i],
                   grid[i + dir_check(msg.direction, grid, width)]);
            clients[grid[i] - 'A'] = 0;
            clients[grid[i + dir_check(msg.direction, grid, width)] - 'A'] = 0;
            grid[i] = ' ';
            grid[i + dir_check(msg.direction, grid, width)] = ' ';
            break;
          }
        }
        /* move without crash */
      } else if (move(msg.client_id, msg.direction, grid, width, size) == 2) {
        /* Client is found on grid */
        for (int i = 0; i < size; ++i) {
          if (grid[i] == msg.client_id) {
            printf("%c moved of board and was destroyed!\n", msg.client_id);
            clients[grid[i] - 'A'] = 0;
            grid[i] = ' ';
            break;
          }
        }
      } else if (move(msg.client_id, msg.direction, grid, width, size) == 1) {
        /* Client is found on grid */
        for (int i = 0; i < size; ++i) {
          if (grid[i] == msg.client_id) {
            printf("%c moved in the direction of %c \n", msg.client_id,
                   msg.direction);
            grid[i + dir_check(msg.direction, grid, width)] = msg.client_id;
            grid[i] = ' ';
            break;
          }
        }
      }
      /* 'i' initializes a client */
    }
    /* print the board */
    printf("\n");
    for (int y = 0; y < height + 2; ++y) {
      for (int x = 0; x < width + 2; ++x) {
        printf("%c", grid[y * (width + 2) + x]);
      }
      printf("\n");
    }
    printf("\n");
    /* till here */
  }
  /* sends message to griddisplay*/ /*
   if (msgsnd(msgid,&msg,sizeof(msg)-sizeof(long), 0) == -1)
   {
         // error handling
         fprintf(stderr,"%s: Can't send message\n",prog_name);
         return EXIT_FAILURE;
   }
   printf("Message sent: %s\n",msg.mText);

 }*/
  // Das muss ausgeführt werden wenn der gridserver beendet wird!
  // msgctl(msgid, IPC_RMID, (struct msqid_ds *) 0);
  free(grid);
  return EXIT_SUCCESS;
}
void sig_handler() {
  printf("Info %s: Exiting...\n", prog_name);
  if(msgid != -1){
    printf("Info %s: Cleaning up the message queue...\n", prog_name);
    msgctl(msgid, IPC_RMID, (struct msqid_ds *) 0);
  }
  printf("Info %s: Freeing memory...\n", prog_name);
  free(grid);
  free(prog_name);
  exit(EXIT_SUCCESS);
}
bool on_board(char id, long clients[]) {
  if (clients[id - 'A'] != 0) {
    return 1;
  }
  return 0;
}

int dir_check(char dir, char grid[], int width) {
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

int move(char id, char dir, char grid[], int width, int size) {
  for (int i = 0; i < size; ++i) {
    if (grid[i] == id) {
      if (grid[i + dir_check(dir, grid, width)] == '#') {
        return 2;
      }
      if (grid[i + dir_check(dir, grid, width)] == ' ') {
        return 1;
      } else {
        return 0;
      }
    }
  }
  return 3;
}
