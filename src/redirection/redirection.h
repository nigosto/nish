#ifndef REDIRECTION_HPP
#define REDIRECTION_HPP

#include "../lexer/lexer.h"

#define NO_REDIRECTION 0
#define IN_REDIRECTION 1
#define OUT_REDIRECTION 2

typedef int redirection_t;

char* extract_file(lexemes_t, redirection_t);

void redirect_input(char*, redirection_t);

void redirect_output(char*, redirection_t);

#endif