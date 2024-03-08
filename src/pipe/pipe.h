#ifndef HPP_PIPE
#define HPP_PIPE

#include "../parser/parser.h"

int* create_pipe_fds(command_list_t);

void close_pipe_fds(int*, size_t);

#endif
