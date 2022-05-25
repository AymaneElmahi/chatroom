
#ifndef TOOLS_H
#define TOOLS_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include "colors.h"
#include "message.h"

// client connection information
typedef struct info
{
  int socket;
  struct sockaddr_in address;
  char username[20];
} info;

void cleanbuffer();

void delete_line(char *input);

// // Splits a string into an array of strings based on a delimiter.
// int str_split(char *a_str, const char a_delim, char **result);

#endif
