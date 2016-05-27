#include <string.h>

#include "autobahn.h"
#include "rlutil.h"

int on_board(char id, char grid[], int size){
  for(int i = 0; i < size; ++i ){
    if(grid[i] == id){
      return 1;
    }
  }
  return 0;
}
/* returns value for movin in 1D array */
int dir_check(char dir, char grid[], int width){
  if(dir == 'N'){
    return (-(width + 2));
  }else if(dir == 'E'){
    return 1;
  }else if(dir == 'S'){
    return (width + 2);
  }else if (dir == 'W'){
    return -1;
  }else{
    return 0;
  }
}
/* returns 1 if move is possible without crash, 2 if move is of grid
   and 0 if move results in crash */
int move(char id, char dir, char grid[], int width, int size){
  for(int i = 0; i < size; ++i ){
    if(grid[i] == id){
      if(grid[i + dir_check(dir, grid, width)] == '#'){
        return 2;
      }
      if(grid[i + dir_check(dir, grid, width)]==' '){
        return 1;
      }else{
        return 0;
      }
    }
  }
  return 3;
}

int main(int argc, char* argv[]){
  int height = 0;
  int width = 0;
  if(argc != 5){
    height = 10;
    width = 10;
    printf("No correct input was made. The grid size was chosen at 10 x 10\n");
    printf("Correct inputlooks like this: ./gridserver -x 10 -y 10\n");
  }else{
    if((strcmp(argv[1], "-x") == 0) && (strcmp(argv[3], "-y") == 0)){
      height = atoi((argv[4]));
      width = atoi(argv[2]);

    }else if((strcmp(argv[3], "-x") == 0) && (strcmp(argv[1], "-y") == 0)){
      height = atoi((argv[2]));
      width = atoi(argv[4]);
    }
  }
  int size = (width + 2) * (height + 2);

  char *grid;
  grid = (char *) malloc( (size) * sizeof(char) );
  /* the board is written */
  for(int i = 0; i < size; ++i){
    if(i < width + 2 || i > (size - (width + 2))){
      grid[i] = '#';
    }else if((i %(width + 2) == (width + 1)) || (i %(width + 2) == 0)){
      grid[i] = '#';
    }else{
      grid[i] = ' ';
    }
  }


  /*
  for(int y = 0; y <= height; ++y){
    for(int x = 0; x <= width; ++x){
      printf("%c", grid[y * (width+1) + x]);
    }
    printf("\n");
  }
  */

  	 /* Buffer fuer navigation am grid */
  navigation msg;

  int msgid = -1;	 /* Message Queue ID */

  /* Message Queue neu anlegen */
  if( (msgid = msgget(KEY,PERM | IPC_CREAT | IPC_EXCL ))==-1 ){
    // error handling
    fprintf(stderr,"%s: Error creating message queue\n",argv[0]);
    return EXIT_FAILURE;
  }


  /* In einer Endlosschleife Nachrichten empfangen */
  while (1){
    if (msgrcv(msgid,&msg,sizeof(msg), 0 , 0) == -1){
      // error handling
      fprintf(stderr,"%s: Can't receive from message queue\n",argv[0]);
      return EXIT_FAILURE;
    }

    printf("Message received: Client ID: %c Client direction: %c\n", msg.client_id , msg.direction);

    if(on_board(msg.client_id, grid, size) && msg.direction == 'T'){
      for(int i = 0; i < size; ++i){
        if(grid[i] == msg.client_id){
          printf("%c was terminated by User\n", msg.client_id);
          grid[i] = ' ';
          grid[i+dir_check(msg.direction, grid, width)] = ' ';
        }
      }
    }else if(on_board(msg.client_id, grid, size)){
      printf("Already on grid\n");
      /* move with crash */
      if(move(msg.client_id, msg.direction, grid, width, size) == 0){
        /* Client is found on grid */
        for(int i = 0; i < size; ++i){
          if(grid[i] == msg.client_id){
            printf("A crash occoured! %c and %c where destroyed!\n", grid[i],grid[i+dir_check(msg.direction, grid, width)]);
            grid[i] = ' ';
            grid[i+dir_check(msg.direction, grid, width)] = ' ';
            break;
          }
        }
      /* move without crash */
      }else if(move(msg.client_id, msg.direction, grid, width, size) == 2){
        /* Client is found on grid */
        for(int i = 0; i < size; ++i){
          if(grid[i] == msg.client_id){
            printf("%c moved of board and was destroyed!\n", msg.client_id);
            grid[i] = ' ';
            break;
          }
        }
      }else if(move(msg.client_id, msg.direction, grid, width, size) == 1){
      /* Client is found on grid */
        for(int i = 0; i < size; ++i){
          if(grid[i] == msg.client_id){
            printf("%c moved in the direction of %c \n",msg.client_id, msg.direction);
            grid[i+dir_check(msg.direction, grid, width)] = msg.client_id;
            grid[i] = ' ';
            break;
          }
        }
      }
    }else{
      fflush(stdout);
      printf("New\n");
      for(int i = 0; i < size; ++i ){
        if(grid[i]==' '){
          grid[i] = msg.client_id;
          break;
        }
      }
    }
    /* print the board */
    for(int i = 0;i<width + 2; ++i){
      printf("-");
    }
    printf("\n");
    for(int y = 0; y < height + 2; ++y){
      for(int x = 0; x < width + 2; ++x){
        printf("%c", grid[y * (width + 2) + x]);
      }
      printf("\n");
    }
    for(int i = 0;i<width + 2; ++i){
      printf("-");
    }
    printf("\n");
    /* till here */
  }
    /* sends message to griddisplay*//*
    if (msgsnd(msgid,&msg,sizeof(msg)-sizeof(long), 0) == -1)
    {
          // error handling
          fprintf(stderr,"%s: Can't send message\n",argv[0]);
          return EXIT_FAILURE;
    }
    printf("Message sent: %s\n",msg.mText);

  }*/
  free(grid);
  return EXIT_SUCCESS;
}
