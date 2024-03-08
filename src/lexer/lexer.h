#ifndef LEXER_HPP
#define LEXER_HPP

#include <stddef.h>

typedef struct lexemes lexemes_s;

typedef lexemes_s* lexemes_t;

char* at(lexemes_t, size_t);

size_t size(lexemes_t);

char* read_command();

lexemes_t parse_lexems(char* command);

lexemes_t init_lexemes();

void push_lexeme(lexemes_t, char*);

void free_lexemes(lexemes_t);

void print_lexemes(lexemes_t);

char** to_command(lexemes_t);

void remove_lexemes(lexemes_t, size_t);

#endif