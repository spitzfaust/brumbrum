#ifndef AUTOBAHN_H
#define AUTOBAHN_H
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include "rlutil.h"

#define KEY 42		/* eindeutiger Key z.B. Matrikelnummer */
#define PIPE_DISPLAY "fourtytwo"
#define PERM 0660
#define SERVER 42L
#define MAX_DATA 255
#define REG_DOUBLE -2
#define REG_FULL -1
#define REG_OK 1

#define BG_RED "\033[41m"

void clear_eol(){
  printf("\x1B[K\n");
}

//Clear EOL = \x1B[K

typedef struct{
  long msg_to;
  long msg_from;
  char client_id;
  char command;
} navigation;

typedef struct{
  long msg_to;
  long msg_from;
  char client_id;
  int x;
  int y;
  int status;
} position;

#endif
