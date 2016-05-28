#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define KEY 4242		/* eindeutiger Key z.B. Matrikelnummer */
#define PERM 0660
#define SERVER 42L
#define MAX_DATA 255


typedef struct{
  long msg_to;
  long msg_from;
  char client_id;
  char direction;
} navigation;

typedef struct{
  long msg_to;
  long msg_from;
  char client_id;
  int x;
  int y;
} position;
