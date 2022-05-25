/*
 *
 * Chatroom - a simple linux commandline client/server C program for group chat.
 * Author: Andrew Herriot
 * License: Public Domain
 *
 */

#ifndef CHATROOM_UTILS_H_
#define CHATROOM_UTILS_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

// color codes
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"
#define RESET "\033[0m"

// Enum of different messages possible.
typedef enum
{
  CONNECT,
  DISCONNECT,
  GET_USERS,
  SET_USERNAME,
  PUBLIC_MESSAGE,
  PRIVATE_MESSAGE,
  TOO_FULL,
  USERNAME_ERROR,
  SUCCESS,
  ERROR,
  GROUP_MESSAGE

} message_type;

// message structure
typedef struct
{
  message_type type;
  char username[21];
  char data[256];
  // char **group;      /*for private_group_message*/
  // int group_size;    /*for private_group_message

} message;

// structure to hold client connection information
typedef struct info
{
  int socket;
  struct sockaddr_in address;
  char username[20];
} info;

// Removes the trailing newline character from a string.
void trim_newline(char *text);

// discard any remaining data on stdin buffer.
void clear_stdin_buffer();

// Splits a string into an array of strings based on a delimiter.
int str_split(char *a_str, const char a_delim, char **result);

#endif
