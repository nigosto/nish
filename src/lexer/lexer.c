#include "lexer.h"
#include "../utils/utils.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct lexemes {
  char **data;
  size_t size, capacity;
} lexemes_s;

void assign_input(char **dest, const char *src, int count, int alloc) {
  char *tmp_dest;

  strncpy(*dest, src, count);
  (*dest)[count] = '\0';
  tmp_dest = realloc(*dest, 256 * alloc * sizeof(char));

  if (!tmp_dest) {
    write(2, "Error: not enough memory!", 26);
    exit(EXIT_FAILURE);
  }

  *dest = tmp_dest;
}

char *read_command() {
  char buffer[256] = "", *command, *tmp_command;
  int count = 0, alloc = 1;

  command = malloc(256 * alloc * sizeof(char));

  while ((count = read(0, buffer, 255)) && !strchr(buffer, '\n')) {
    assign_input(&command, buffer, count, ++alloc);
  }

  if (strchr(buffer, '\n')) {
    --count;
  }
  assign_input(&command, buffer, count, ++alloc);

  return command;
}

lexemes_t init_lexemes() {
  lexemes_t lexemes = malloc(sizeof(lexemes_s));

  lexemes->data = malloc(sizeof(char*));
  lexemes->size = 0;
  lexemes->capacity = 1;

  return lexemes;
}

char* at(lexemes_t lexemes, size_t index) {
  if(index >= lexemes->size) {
    write(2, "Error: index out of range!", 27);
    exit(EXIT_FAILURE);
  }

  return lexemes->data[index];
}

size_t size(lexemes_t lexemes) {
  return lexemes->size;
}

void push_lexeme(lexemes_t lexemes, char* token) {
  if(lexemes->size == lexemes->capacity) {
    lexemes->capacity *= 2;
    char** tmp_data = realloc(lexemes->data, lexemes->capacity * sizeof(char*));
    
    if(!tmp_data) {
      write(2, "Error: not enough memory!", 26);
      exit(EXIT_FAILURE);
    }

    lexemes->data = tmp_data;
  }

  lexemes->data[lexemes->size++] = token;
}

void free_lexemes(lexemes_t lexemes) {
  free(lexemes->data);
  free(lexemes);
}

lexemes_t parse_lexems(char* command) {
  int i = 0;
  char prev = ' ';
  lexemes_t lexemes = init_lexemes();

  while (command[i]) {
    if (delimiter(command[i])) {
      prev = command[i];
      command[i] = '\0';
    } else if (delimiter(prev)) {
      push_lexeme(lexemes, command + i);
      prev = command[i];
    }
    ++i;
  }

  return lexemes;
}

void print_lexemes(lexemes_t lexemes) {
  int i;
  for (i = 0; i < lexemes->size; ++i) {
    printf("%s ", lexemes->data[i]);
  }
  printf("\n");
}

char** to_command(lexemes_t lexemes) {
  char** command = malloc((size(lexemes) + 1) * sizeof(char*));
  command[size(lexemes)] = NULL;

  int i;
  for (i = 0; i < size(lexemes); ++i) {
    command[i] = lexemes->data[i];
  }

  return command;
}