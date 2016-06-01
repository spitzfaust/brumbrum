//////////////////////
// OBES IPC EXERCISE //
//      BY          //
// THOMAS RAUHOFER  //
//     AND          //
//  TOBIAS WATZEK   //
//////////////////////
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

// unique key for the msg queue
#define KEY 4224
// name of the fifo
#define PIPE_DISPLAY "fourtytwo"
// perms for msg queue
#define PERM 0660
// server id
#define SERVER 42L
// double registration return value
#define REG_DOUBLE -2
// full registration return value
#define REG_FULL -1
// registration ok return value
#define REG_OK 1

// just for the eyes - key for red bgcolor
#define BG_RED "\033[41m"
/**
 * Clear until the end of the line is reached and add a line break at the end
 * Used to make lines with filled backgroundcolor
 */
void clear_eol() {
  printf("\x1B[K\n");
}

/**
 * navigation struct
 * Used to communicate navigation commands from the client to the server
 */
typedef struct {
  //  receiver of the msg
  long msg_to;
  // sender of the msg (pid of the client)
  long msg_from;
  // id of the client (uppercase letter)
  char client_id;
  // command (N, E, S, W, T, i)
  char command;
} navigation;

/**
 * position struct
 * Used to communicate start position of a new client from the server
 */
typedef struct {
  // receiver of the msg (pid of the client)
  long msg_to;
  // sender of the msg
  long msg_from;
  // id of the client (uppercase letter)
  char client_id;
  // x pos on the grid
  int x;
  // y pos on the grid
  int y;
  // registration status (REG_DOUBLE, REG_FULL, REG_OK)
  int status;
} position;

#endif
