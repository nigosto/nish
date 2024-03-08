
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "pipe/pipe.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  while (1) {
    int out = dup(1), status;
    
    write(1, "> ", 2);

    char *command = read_command();
    lexemes_t lexems = parse_lexems(command);

    if (end_session(lexems)) {
      write(1, "Exitting command interpreter...\n", 33);
      free(command);
      free_lexemes(lexems);
      break;
    }

    command_list_t command_list = separate_commands(lexems);
    size_t commands_count = commands_size(command_list);

    int* pipes = create_pipe_fds(command_list);

    if (fork() == 0) {
      if (commands_count > 1) {
        close(1);
        dup(pipes[1]);
        close_pipe_fds(pipes, commands_count);
      }

      execute_command(command_list, 0, out);
    } else {
      if (commands_count > 1) {
        close(pipes[1]);
      }
      int child = wait(&status);
    }

    int i;
    for (i = 1; i < commands_count - 1; ++i) {
      if (fork() == 0) {
        close(0);
        dup(pipes[(i - 1) * 2]);
        close(1);
        dup(pipes[i * 2 + 1]);

        close(pipes[i * 2]);
        close(pipes[i * 2 + 1]);

        execute_command(command_list, i, out);
      } else {
        close(pipes[(i - 1) * 2 + 1]);

        int child = wait(&status);
      }
    }

    if (commands_count > 1) {
      if (fork() == 0) {
        close(0);
        dup(pipes[(commands_count - 2) * 2]);
        close_pipe_fds(pipes, commands_count);

        execute_command(command_list, commands_count - 1, 1);
      } else {
        close_pipe_fds(pipes, commands_count);
      }
    }

    free(pipes);

    free_commands_list(command_list);

    free(command);
    free_lexemes(lexems);
  }

  return 0;
}