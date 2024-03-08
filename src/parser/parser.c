#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct command_list {
  lexemes_t *data;
  size_t size, capacity;
} command_list_s;

bool end_session(lexemes_t tokens) {
  return strcmp(at(tokens, 0), "exit") == 0;
}

command_list_t init_commands_list() {
  command_list_t commands_list = malloc(sizeof(command_list_s));

  commands_list->data = malloc(sizeof(lexemes_t));
  commands_list->size = 0;
  commands_list->capacity = 1;

  return commands_list;
}

void push_command(command_list_t command_list, lexemes_t lexemes) {
  if (command_list->size == command_list->capacity) {
    command_list->capacity *= 2;
    lexemes_t *tmp_data =
        realloc(command_list->data, command_list->capacity * sizeof(lexemes_t));

    if (!tmp_data) {
      write(2, "Error: not enough memory!", 26);
      exit(EXIT_FAILURE);
    }

    command_list->data = tmp_data;
  }

  command_list->data[command_list->size++] = lexemes;
}

command_list_t separate_commands(lexemes_t lexemes) {
  int i, prev = 0;
  size_t lexemes_count = size(lexemes);
  command_list_t command_list = init_commands_list();

  for (i = 0; i < lexemes_count; ++i) {
    if (strcmp(at(lexemes, i), "|") == 0) {
      int k, index = command_list->size;
      push_command(command_list, init_lexemes());
      for (k = prev; k < i; ++k) {
        push_lexeme(command_list->data[index], at(lexemes, k));
      }
      prev = i + 1;
    }
  }

  int k, index = command_list->size;
  push_command(command_list, init_lexemes());
  for (k = prev; k < i; ++k) {
    push_lexeme(command_list->data[index], at(lexemes, k));
  }

  return command_list;
}

size_t commands_size(command_list_t command_list) { return command_list->size; }

void free_commands_list(command_list_t command_list) {
  int i;
  for (i = 0; i < command_list->size; ++i) {
    free_lexemes(command_list->data[i]);
  }
  free(command_list->data);
  free(command_list);
}

void execute_command(command_list_t command_list, size_t index, int error) {
  lexemes_t lexemes = command_list->data[index];
  char** command = to_command(lexemes);

  if (execvp(at(lexemes, 0), command) == -1) {
    write(error, "\nInvalid command\n", 17);

    exit(1);
  }
}

redirection_t check_for_redirection(lexemes_t lexemes) {
  size_t count = size(lexemes);

  if (count < 3) {
    return NO_REDIRECTION;
  }

  if (strcmp(at(lexemes, count - 2), ">") == 0) {
    return OUT_REDIRECTION;
  } else if (strcmp(at(lexemes, count - 2), "<") == 0) {
    return IN_REDIRECTION;
  } else {
    return NO_REDIRECTION;
  }
}

bool check_for_background_process(lexemes_t lexemes) {
  size_t count = size(lexemes);

  return at(lexemes, count - 1)[0] == '&';
}