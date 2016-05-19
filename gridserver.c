#include "myqueue.h"

int main(int argc, char* argv[])
{
   message_t msg;	/* Buffer fuer Message */
   int msgid = -1;	/* Message Queue ID */

   /* Message Queue neu anlegen */
   if( (msgid = msgget(KEY,PERM | IPC_CREAT | IPC_EXCL ))==-1 )
   {
      /* error handling */
      fprintf(stderr,"%s: Error creating message queue\n",argv[0]);
      return EXIT_FAILURE;
   }

   /* In einer Endlosschleife Nachrichten empfangen */
   while (1)
   {
      if (msgrcv(msgid,&msg,sizeof(msg)-sizeof(long), 0 , 0) == -1)
      {
         /* error handling */
         fprintf(stderr,"%s: Can't receive from message queue\n",argv[0]);
         return EXIT_FAILURE;
      }
      printf("Message received: %s\n",msg.mText);
      
      /* sends message to griddisplay*//*
      if (msgsnd(msgid,&msg,sizeof(msg)-sizeof(long), 0) == -1)
      {
            // error handling
            fprintf(stderr,"%s: Can't send message\n",argv[0]);
            return EXIT_FAILURE;
      }
      printf("Message sent: %s\n",msg.mText);
      */
   }
   return EXIT_SUCCESS;
}
