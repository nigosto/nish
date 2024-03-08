#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define NO_REDIRECTION 0
#define IN_REDIRECTION 1
#define OUT_REDIRECTION 2

void read_command(char cmd[]) {
  char buffer[2] = "";
  int count = 0, i = 0;
  while ((count = read(0, buffer, 1)) && buffer[0] != '\n') {
    cmd[i++] = buffer[0];
  }
}

int delimiter(char c) { return c == ' ' || c == '\t' || c == '\n'; }

int parse_lexems(char cmd[], char *args[]) {
  int i = 0, argc = 0;
  char prev = ' ';

  while (cmd[i]) {
    if (delimiter(cmd[i])) {
      prev = cmd[i];
      cmd[i] = '\0';
    } else if (delimiter(prev)) {
      args[argc++] = cmd + i;
      prev = cmd[i];
    }
    ++i;
  }

  args[argc] = NULL;
  return argc;
}

int find(int arr[], int size, int el) {
  int i = 0;
  for (i = 0; i < size; ++i) {
    if (arr[i] == el) {
      return 1;
    }
  }

  return 0;
}

int separate_commands(char *args[], int argc, char *commands[][255]) {
  int i, j = 0, prev = 0;
  for (i = 0; i < argc; ++i) {
    if (strcmp(args[i], "|") == 0) {
      int k;
      for (k = prev; k < i; ++k) {
        commands[j][k - prev] = args[k];
      }
      commands[j][i - prev] = NULL;
      prev = i + 1;
      ++j;
    }
  }

  int k;
  for (k = prev; k < i; ++k) {
    commands[j][k - prev] = args[k];
  }
  commands[j][i - prev] = NULL;
  ++j;

  return j;
}

int check_for_redirection(char *args[], int argc) {
  if (argc < 3) {
    return NO_REDIRECTION;
  }

  if (strcmp(args[argc - 2], ">") == 0) {
    return OUT_REDIRECTION;
  } else if (strcmp(args[argc - 2], "<") == 0) {
    return IN_REDIRECTION;
  } else {
    return NO_REDIRECTION;
  }
}

int main() {
  int status;

  while (1) {
    write(1, "> ", 2);
    char cmd[255] = "";
    read_command(cmd);
    char *args[255];
    int argc = parse_lexems(cmd, args);

    if (strcmp(args[0], "exit") == 0) {
      write(1, "Exitting command interpreter...\n", 33);
      break;
    }

    int background_process = args[argc - 1][0] == '&';
    if (background_process) {
      args[argc - 1] = NULL;
      --argc;
    }

    char *file = NULL;
    int redirection = check_for_redirection(args, argc);
    if (redirection != NO_REDIRECTION) {
      file = args[argc - 1];
      argc -= 2;
    }

    char *commands[255][255];
    int commands_count = separate_commands(args, argc, commands);

    int i;
    int fd[100];

    if (commands_count > 1) {
      for (i = 0; i < commands_count - 1; ++i) {
        pipe(fd + i * 2);
      }
    }

    int out = dup(1);

    if (fork() == 0) {
      if (redirection == IN_REDIRECTION) {
        close(0);
        if (open(file, O_RDONLY) == -1) {
          exit(1);
        }
      }

      if (commands_count > 1) {
        close(1);
        dup(fd[1]);
        for (i = 0; i < commands_count - 1; ++i) {
          close(fd[i * 2]);
          close(fd[i * 2 + 1]);
        }
      } else {
        if (redirection == OUT_REDIRECTION) {
          close(1);
          if (open(file, O_WRONLY | O_CREAT | O_TRUNC, 0744) == -1) {
            exit(1);
          }
        }
      }

      if (execvp(commands[0][0], commands[0]) == -1) {
        write(out, "\nInvalid command\n", 17);

        exit(1);
      }
    } else {
      if (commands_count > 1) {
        close(fd[1]);
      }
      int child = wait(&status);
    }

    for (i = 1; i < commands_count - 1; ++i) {
      if (fork() == 0) {
        close(0);
        dup(fd[(i - 1) * 2]);
        close(1);
        dup(fd[i * 2 + 1]);

        close(fd[i * 2]);
        close(fd[i * 2 + 1]);

        if (execvp(commands[i][0], commands[i]) == -1) {
          write(out, "\nInvalid command\n", 17);

          exit(1);
        }
      } else {
        close(fd[(i - 1) * 2 + 1]);

        int child = wait(&status);
      }
    }

    if (commands_count > 1) {
      if (fork() == 0) {
        close(0);
        dup(fd[(commands_count - 2) * 2]);
        for (i = 0; i < commands_count - 1; ++i) {
          close(fd[i * 2]);
          close(fd[i * 2 + 1]);
        }

        if (redirection == OUT_REDIRECTION) {
          close(1);
          if (open(file, O_WRONLY | O_CREAT | O_TRUNC, 0744) == -1) {
            exit(1);
          }
        }

        if (execvp(commands[commands_count - 1][0],
                   commands[commands_count - 1]) == -1) {
          write(1, "\nInvalid command\n", 17);

          exit(1);
        }
      } else {
        for (i = 0; i < commands_count - 1; ++i) {
          close(fd[i * 2]);
          close(fd[i * 2 + 1]);
        }

        if (!background_process) {
          int child = wait(&status);
        }
      }
    }
  }

  return 0;
}