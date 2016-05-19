#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define KEY 3232323232		/* eindeutiger Key z.B. Matrikelnummer */
#define PERM 0660


typedef struct
{
  char client_id;
  char direction;
} navigation;

typedef struct
{
  char client_id;
	int x;
  int y;
} position;
