#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include "autobahn.h"

int main() {
  char incoming[DISPLAY_BUFFER];
  int display = open(PIPE_DISPLAY, O_RDONLY);

  while (1) {
    if (read(display, incoming, DISPLAY_BUFFER)) {
      printf("%s", incoming);
    }
  }

  return 0;
}
