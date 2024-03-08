#include "pipe.h"
#include <stdlib.h>
#include <unistd.h>

int *create_pipe_fds(command_list_t commands_list) {
  size_t size = commands_size(commands_list);
  int i, *fd = malloc((size - 1) * 2 * sizeof(int));

  if (size > 1) {
    for (i = 0; i < size - 1; ++i) {
      pipe(fd + i * 2);
    }
  }

  return fd;
}

void close_pipe_fds(int *pipes, size_t count) {
  int i;
  for (i = 0; i < count - 1; ++i) {
    close(pipes[i * 2]);
    close(pipes[i * 2 + 1]);
  }
}

void pipe_output(int* pipes, int index) {
  close(1);
  dup(pipes[index]);
}

void pipe_input(int* pipes, int index) {
  close(0);
  dup(pipes[index]);
}