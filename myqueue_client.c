#include "myqueue.h"

int main(int argc, char* argv[])
{
   message_t msg;	/* Buffer fuer Message */
   int msgid = -1;	/* Message Queue ID */

   /* Argument Handling */
   if (argc!=2)
   {
      fprintf(stderr,"Usage: %s <Message>\n",argv[0]);
      return EXIT_FAILURE;
   }
   
   /* Message Queue oeffnen */
   if( (msgid = msgget(KEY,PERM))==-1 )
   {
      /* error handling */
      fprintf(stderr,"%s: Can't access message queue\n",argv[0]);
      return EXIT_FAILURE;
   }

   /* Nachricht verschicken */
   msg.mType=1;
   strncpy(msg.mText,argv[1],MAX_DATA);
   if (msgsnd(msgid,&msg,sizeof(msg)-sizeof(long), 0) == -1)
   {
         /* error handling */
         fprintf(stderr,"%s: Can't send message\n",argv[0]);
         return EXIT_FAILURE;
   }
   printf("Message sent: %s\n",msg.mText);
   return EXIT_SUCCESS;
}


