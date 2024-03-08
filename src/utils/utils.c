#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

bool delimiter(char c) { return c == ' ' || c == '\t' || c == '\n'; }

void display_pid(int out, int pid) {
  char text[255] = "";
  sprintf(text, "[%d]\n", pid);
  write(out, text, strlen(text));
}