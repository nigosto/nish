#ifndef PARSER_HPP
#define PARSER_HPP

#include "../libs/bool.h"
#include "../lexer/lexer.h"

typedef struct command_list command_list_s;

typedef command_list_s* command_list_t ;

bool end_session(lexemes_t);

command_list_t separate_commands(lexemes_t);

void push_command(command_list_t, lexemes_t);

size_t commands_size(command_list_t);

void free_commands_list(command_list_t);

void print_commands(command_list_t);

void execute_command(command_list_t, size_t, int);

#endif