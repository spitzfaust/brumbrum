#include <string.h>

#include "autobahn.h"
#include "rlutil.h"

int main(int argc, char* argv[]){
  int height = 0;
  int width = 0;
  if((strcmp(argv[1], "-x") == 0) && (strcmp(argv[3], "-y") == 0)){
    height = atoi((argv[4]));
    width = atoi(argv[2]);

  }else if((strcmp(argv[3], "-x") == 0) && (strcmp(argv[1], "-y") == 0)){
    height = atoi((argv[2]));
    width = atoi(argv[4]);
  }
  int size = (width + 1) * (height + 1);

  char *grid;
  grid = (char *) malloc( (size) * sizeof(char) );
 /* the board is written */
  for(int i = 0; i < size; ++i){
    if((i % (width + 1)) == 0){
      grid[i] = 'r';
    }else{
      grid[i] = ' ';
    }
  }
  for(int i = size - (width + 1); i < size +1; ++i){
    if(i == size - (width +1) ){
      grid[i] = 'O';
    }else{
      grid[i] = 'c';
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

  navigation navi[27];	 /* Buffer fuer navigation am grid */
  navigation msg;
  navi[26].client_id = '0';
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
    printf("Message received: Client ID:%s Client direction:%s\n", msg.client_id , msg .direction);
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
