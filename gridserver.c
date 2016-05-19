#include "autobahn.h"

int main(int argc, char* argv[])
{
  int height;
  int width;
  if((strcmp("-x", argv[1]) == 0) && (strcmp("-y", argv[3]) == 0)){
    height = *argv[4];
    width = *argv[2];
  }else if((strcmp("-y", argv[1]) == 0) && (strcmp("-x", argv[3]) == 0)){
    height = *argv[2];
    width = *argv[4];
  }
  int size = 5 * 4;

  char *grid;
  grid = (char *) malloc( (size) * sizeof(char) );

  for(int i = 0; i < size; ++i){
    grid[i] = 'X';
  }
  printf("FUCK");
  for(int y = 0; y < height; ++y){
    for(int x = 0; x < width; ++x){
      printf("%c", grid[y*(height - 1) + x]);
    }
    printf("\n");
  }

  navigation navi[26];	/* Buffer fuer navigation am grid */
  int msgid = -1;	/* Message Queue ID */

  /* Message Queue neu anlegen */
  if( (msgid = msgget(KEY,PERM | IPC_CREAT | IPC_EXCL ))==-1 )
  {
    /* error handling */
    fprintf(stderr,"%s: Error creating message queue\n",argv[0]);
    return EXIT_FAILURE;
  }


  /* In einer Endlosschleife Nachrichten empfangen */
  /*while (1)
  {
    if (msgrcv(msgid,&msg,sizeof(msg)-sizeof(long), 0 , 0) == -1)
    {
      // error handling
      fprintf(stderr,"%s: Can't receive from message queue\n",argv[0]);
      return EXIT_FAILURE;
    }*/
    //printf("Message received: %s\n",msg.mText);

    /* sends message to griddisplay*//*
    if (msgsnd(msgid,&msg,sizeof(msg)-sizeof(long), 0) == -1)
    {
          // error handling
          fprintf(stderr,"%s: Can't send message\n",argv[0]);
          return EXIT_FAILURE;
    }
    printf("Message sent: %s\n",msg.mText);

  }*/
  return EXIT_SUCCESS;
}
