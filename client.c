#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include "tools.h"

// get a username from the user.
void get_username(char *username)
{
  while (1)
  {
    printf("Enter a username: ");
    fflush(stdout);
    memset(username, 0, 1000);
    fgets(username, 22, stdin);
    trim_newline(username);

    if (strlen(username) > 20)
    {

      puts("Username must not be longer than 20 characters \n.");
    }
    else
    {
      break;
    }
  }
}

// send local username to the server.
void set_username(info *connection)
{
  message msg;
  msg.type = SET_USERNAME;
  strncpy(msg.username, connection->username, 20);

  if (send(connection->socket, (void *)&msg, sizeof(msg), 0) < 0)
  {
    perror("Send failed");
    exit(1);
  }
}

void stop_client(info *connection)
{
  close(connection->socket);
  exit(0);
}

// initialize connection to the server.
void connect_to_server(info *connection, char *address, char *port)
{

  while (1)
  {
    get_username(connection->username);

    // Create socket
    if ((connection->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
      perror("Could not create socket");
    }

    connection->address.sin_addr.s_addr = inet_addr(address);
    connection->address.sin_family = AF_INET;
    connection->address.sin_port = htons(atoi(port));

    // Connect to remote server
    if (connect(connection->socket, (struct sockaddr *)&connection->address, sizeof(connection->address)) < 0)
    {
      perror("Connect failed.");
      exit(1);
    }

    set_username(connection);

    message msg;
    ssize_t recv_val = recv(connection->socket, &msg, sizeof(message), 0);
    if (recv_val < 0)
    {
      perror("recv failed");
      exit(1);
    }
    else if (recv_val == 0)
    {
      close(connection->socket);
      printf("The username %s is already used, please choose another one\n", connection->username);
      continue;
    }

    break;
  }

  printf("Connected to server.\n");
  printf("Type /help for usage.\n");
}

void handle_user_input(info *connection)
{
  char input[255];
  fgets(input, 255, stdin);
  trim_newline(input);

  if (strcmp(input, "/q") == 0 || strcmp(input, "/quit") == 0)
  {
    stop_client(connection);
  }
  else if (strcmp(input, "/l") == 0 || strcmp(input, "/list") == 0)
  {
    message msg;
    msg.type = GET_USERS;

    if (send(connection->socket, &msg, sizeof(message), 0) < 0)
    {
      perror("Send failed");
      exit(1);
    }
  }
  else if (strcmp(input, "/h") == 0 || strcmp(input, "/help") == 0)
  {
    puts("/quit or /q: Exit the program.");
    puts("/help or /h: Show help information.");
    puts("/list or /l: Show list of users in chatroom.");
    puts("/m <username> <message> Send a private message to given username.");
    // puts("/g <username1> <username2> ... <message> Send a private message to given users.");
  }
  else if (strncmp(input, "/m", 2) == 0)
  {

    message msg;
    msg.type = PRIVATE_MESSAGE;

    char *specific_username, *chatMsg;

    specific_username = strtok(input + 3, " ");

    if (specific_username == NULL)
    {
      puts(KRED "The format for private messages is: /m <username> <message>" RESET);
      return;
    }

    if (strlen(specific_username) == 0)
    {
      puts(KRED "You must enter a username for a private message." RESET);
      return;
    }

    if (strlen(specific_username) > 20)
    {
      puts(KRED "The username must not exceed 20 characters." RESET);
      return;
    }

    chatMsg = strtok(NULL, "");

    if (chatMsg == NULL)
    {
      puts(KRED "You must enter a message to send to the specified user." RESET);
      return;
    }

    strncpy(msg.username, specific_username, 20);
    strncpy(msg.data, chatMsg, 255);

    if (send(connection->socket, &msg, sizeof(message), 0) < 0)
    {
      perror("Send failed");
      exit(1);
    }
  }
  // else if (strncmp(input, "/g", 2) == 0)
  // {
  //   message msg;
  //   msg.type = GROUP_MESSAGE;

  //   char **users;
  //   char *chatMsg;

  //   // get the users (the last "user" is the message)
  //   int num_users = str_split(input + 3, ' ', users);

  //   if (num_users <= 1)
  //   {
  //     puts(KRED "You must enter at least one username for a group message." RESET);
  //     return;
  //   }

  //   chatMsg = users[num_users - 1];

  //   if (strlen(chatMsg) == 0)
  //   {
  //     puts(KRED "You must enter a message to send to the specified users." RESET);
  //     return;
  //   }

  //   msg.group_size = num_users - 1;

  //   for (int i = 0; i < num_users - 1; i++)
  //   {
  //     strncpy(msg.group[i], users[i], 20);
  //   }

  //   strncpy(msg.data, chatMsg, 255);

  //   if (send(connection->socket, &msg, sizeof(message), 0) < 0)
  //   {
  //     perror("Send failed");
  //     exit(1);
  //   }

  //   free(users);
  // }
  else // regular public message
  {
    message msg;
    msg.type = PUBLIC_MESSAGE;
    strncpy(msg.username, connection->username, 20);

    if (strlen(input) == 0)
    {
      return;
    }

    strncpy(msg.data, input, 255);

    // Send data
    if (send(connection->socket, &msg, sizeof(message), 0) < 0)
    {
      perror("Send failed");
      exit(1);
    }
  }
}

void handle_server_message(info *connection)
{
  message msg;

  // Receive a reply from the server
  ssize_t recv_val = recv(connection->socket, &msg, sizeof(message), 0);
  if (recv_val < 0)
  {
    perror("recv failed");
    exit(1);
  }
  else if (recv_val == 0)
  {
    close(connection->socket);
    puts("Server disconnected.");
    exit(0);
  }

  switch (msg.type)
  {

  case CONNECT:
    printf(KYEL "%s has connected." RESET "\n", msg.username);
    break;

  case DISCONNECT:
    printf(KYEL "%s has disconnected." RESET "\n", msg.username);
    break;

  case GET_USERS:
    printf("%s", msg.data);
    break;

  case PUBLIC_MESSAGE:
    printf(KGRN "%s" RESET ": %s\n", msg.username, msg.data);
    break;

  case PRIVATE_MESSAGE:
    printf(KWHT "From %s:" KCYN " %s\n" RESET, msg.username, msg.data);
    break;

  case GROUP_MESSAGE:
    printf(KWHT "From %s:" KCYN " %s\n" RESET, msg.username, msg.data);
    break;

  case TOO_FULL:
    fprintf(stderr, KRED "Server chatroom is too full to accept new clients." RESET "\n");
    exit(0);
    break;

  default:
    fprintf(stderr, KRED "Unknown message type received." RESET "\n");
    break;
  }
}

int main(int argc, char *argv[])
{
  info connection;
  fd_set file_descriptors;

  if (argc != 3)
  {
    fprintf(stderr, "Usage: %s <IP> <port>\n", argv[0]);
    exit(1);
  }

  connect_to_server(&connection, argv[1], argv[2]);

  // establish a communication with the server
  while (1)
  {
    FD_ZERO(&file_descriptors);
    FD_SET(STDIN_FILENO, &file_descriptors);
    FD_SET(connection.socket, &file_descriptors);
    fflush(stdin);

    if (select(connection.socket + 1, &file_descriptors, NULL, NULL, NULL) < 0)
    {
      perror("Select failed.");
      exit(1);
    }

    if (FD_ISSET(STDIN_FILENO, &file_descriptors))
    {
      handle_user_input(&connection);
    }

    if (FD_ISSET(connection.socket, &file_descriptors))
    {
      handle_server_message(&connection);
    }
  }

  close(connection.socket);
  return 0;
}
