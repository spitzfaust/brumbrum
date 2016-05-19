/* Test Programm fuer Unnamed Pipes */


#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <limits.h>

int main (int argc, char **argv) {
   int fd[2];
   pid_t pid;
   char puffer[PIPE_BUF];
   FILE *reading, *writing, *newfile;

   if (argc != 2) {
      fprintf(stderr,"Usage: %s Datei\n", argv[0]);
      exit (EXIT_FAILURE);
   }
   if ((newfile = fopen (argv[1], "a+")) < 0) {
      perror ("fopen");
      exit (EXIT_FAILURE);
   }
   /*Wir erstellen eine pipe*/
   if (pipe (fd) < 0) {
      perror ("pipe");
      exit (EXIT_FAILURE);
   }
   /*Wir erzeugen einen neuen Prozess*/
   if ((pid = fork ()) < 0) {
      perror ("fork");
      exit (EXIT_FAILURE);
   }
   else if (pid > 0) {   /*Elternprozess */
      close (fd[0]);   /*Leseseite schliessen */
      if ((writing = fdopen (fd[1], "w")) == NULL) {
         perror ("fdopen");
         exit (EXIT_FAILURE);
      }
      /* PIPE_BUF Bytes lesen */
      while (fgets (puffer, PIPE_BUF, stdin)!=NULL)
      {
         /*Wir schreiben in die Pipe */
         fputs (puffer, writing);
      }
      /* Pipe schliessen */
      fclose(writing);

      /* Warten auf den Kindprozess */
      if ((waitpid (pid, NULL, 0)) < 0) {
         perror ("waitpid");
         exit (EXIT_FAILURE);
      }
   }
   else {     /*Kindprozess */
      close (fd[1]);   /*Schreibseite schliessen */
      if ((reading = fdopen (fd[0], "r")) == NULL) {
         perror ("fdopen");
         exit (EXIT_FAILURE);
      }
      /* Wir lesen aus der Pipe (PIPE_BUF Bytes) */
      while (fgets (puffer, PIPE_BUF, reading)!=NULL)
      {
         fputs (puffer, newfile);
      }
      fclose(newfile);
      fclose(reading);      
   }
   exit (EXIT_SUCCESS);
}
