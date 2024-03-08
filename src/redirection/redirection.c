#include "redirection.h"
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

char *extract_file(lexemes_t lexemes, redirection_t redirection) {
  char *file = NULL;

  if (redirection != NO_REDIRECTION) {
    file = at(lexemes, size(lexemes) - 1);
    remove_lexemes(lexemes, 2);
  }

  return file;
}

void redirect_input(char *file, redirection_t redirection) {
  if (redirection == IN_REDIRECTION) {
    close(0);
    if (open(file, O_RDONLY) == -1) {
      exit(EXIT_FAILURE);
    }
  }
}

void redirect_output(char *file, redirection_t redirection) {
  if (redirection == OUT_REDIRECTION) {
    close(1);
    if (open(file, O_WRONLY | O_CREAT | O_TRUNC, 0744) == -1) {
      exit(EXIT_FAILURE);
    }
  }
}