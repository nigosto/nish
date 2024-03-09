
#include "lexer/lexer.h"
#include "../libs/messages/messages.h"
#include "parser/parser.h"
#include "pipe/pipe.h"
#include "redirection/redirection.h"
#include "utils/utils.h"
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  while (true) {
    int out = dup(1), i;

    write(1, "> ", 2);

    char *command = read_command();
    lexemes_t lexemes = parse_lexems(command);

    if (end_session(lexemes)) {
      write(1, ENDING_SESSION_MESSAGE, strlen(ENDING_SESSION_MESSAGE));
      free(command);
      free_lexemes(lexemes);
      break;
    }

    bool start_in_background = check_for_background_process(lexemes);
    if (start_in_background) {
      remove_lexemes(lexemes, 1);
    }

    redirection_t redirection = check_for_redirection(lexemes);
    char *file = extract_file(lexemes, redirection);

    command_list_t command_list = separate_commands(lexemes);
    size_t commands_count = commands_size(command_list);

    int *pipes = create_pipe_fds(command_list);

    if (fork() == 0) {
      redirect_input(file, redirection);

      if (commands_count > 1) {
        pipe_output(pipes, 1);
        close_pipe_fds(pipes, commands_count);
      } else {
        redirect_output(file, redirection);
      }

      if (start_in_background) {
        display_pid(out, getpid());
      }

      execute_command(command_list, 0, out);
    } else {
      if (commands_count > 1) {
        close(pipes[1]);
      }
      wait(NULL);
    }

    for (i = 1; i < commands_count - 1; ++i) {
      if (fork() == 0) {
        pipe_input(pipes, (i - 1) * 2);
        pipe_output(pipes, i * 2 + 1);

        close(pipes[i * 2]);
        close(pipes[i * 2 + 1]);

        execute_command(command_list, i, out);
      } else {
        close(pipes[(i - 1) * 2 + 1]);
        wait(NULL);
      }
    }

    if (commands_count > 1) {
      if (fork() == 0) {
        pipe_input(pipes, (commands_count - 2) * 2);
        close_pipe_fds(pipes, commands_count);

        redirect_output(file, redirection);

        execute_command(command_list, commands_count - 1, 1);
      } else {
        close_pipe_fds(pipes, commands_count);

        if (!start_in_background) {
          wait(NULL);
        }
      }
    }

    free(pipes);
    free_commands_list(command_list);
    free(command);
    free_lexemes(lexemes);
  }

  return EXIT_SUCCESS;
}