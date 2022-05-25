#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include "tools.h"

/**
 * @brief ask for a string from the user
 *
 * @param username
 */
void ask_for_username(char *username)
{
  int out = 0;
  while (!out)
  {
    // ask for username until it's valid
    printf("Enter a username: ");
    fflush(stdout);
    memset(username, 0, 1000);
    fgets(username, 22, stdin);
    delete_line(username);
    out = 1;
    if (strlen(username) > 20)
    {
      printf("Username can't exceed 20 characters \n.");
      out = 0;
    }
  }
}

/**
 * @brief initialize username
 *
 * @param connection
 */
void initialize_username(info *connection)
{
  // initialize username
  message msg;
  msg.type = USERNAME;
  strncpy(msg.user, connection->username, 20);

  // send username to server
  if (send(connection->socket, (void *)&msg, sizeof(msg), 0) == -1)
  {
    perror("Error send");
    exit(1);
  }
}

/**
 * @brief close connection
 *
 * @param connection
 */
void close_client(info *connection)
{
  close(connection->socket);
  exit(0);
}

/**
 * @brief connect the client to the server
 *
 * @param connection
 * @param address
 * @param port
 */
void connect_server(info *connection, char *address, char *port)
{

  while (1)
  {
    // ask for username
    ask_for_username(connection->username);

    // Create socket
    if ((connection->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
      perror("Could not create socket");
    }

    // Set up address structure
    connection->address.sin_addr.s_addr = inet_addr(address);

    // Set up port number
    connection->address.sin_family = AF_INET;

    // Convert port number to binary
    connection->address.sin_port = htons(atoi(port));

    // Connect to remote server
    if (connect(connection->socket, (struct sockaddr *)&connection->address, sizeof(connection->address)) == -1)
    {
      perror("Error: connect");
      exit(1);
    }

    // initialize username
    initialize_username(connection);

    // initialize message
    message msg;
    ssize_t recv_val = recv(connection->socket, &msg, sizeof(message), 0);
    if (recv_val == -1)
    {
      perror("Error: recv");
      exit(1);
    }
    else if (recv_val == 0)
    {
      close(connection->socket);
      printf("The username %s is taken, choose another one\n", connection->username);
      continue;
    }

    break;
  }

  // print welcome message
  printf("Connected !\n");
  printf("Type /help for usage.\n");
}

/**
 * @brief  handle the client input
 *
 * @param connection
 */
void user_input(info *connection)
{
  // get user input
  char input[255];
  fgets(input, 255, stdin);
  delete_line(input);

  // check if the user wants to quit
  if (!strcmp(input, "/q") || !strcmp(input, "/quit"))
  {
    close_client(connection);
  }
  // check if the user wants users list
  else if (!strcmp(input, "/l") || !strcmp(input, "/list"))
  {
    message msg;
    msg.type = USERS;

    if (send(connection->socket, &msg, sizeof(message), 0) == -1)
    {
      perror("Error send");
      exit(1);
    }
  }
  // check if the user wants to see help
  else if (!strcmp(input, "/h") || !strcmp(input, "/help"))
  {
    printf("/quit or /q: Exit the server.\n");
    printf("/help or /h: Show help .\n");
    printf("/list or /l: Show list of connected users.\n");
    printf("/m <username> <message> Send a private message to a specified user.\n");
    // printf("/g <username1> <username2> ... <message> Send a private message to given users.\n");
  }
  // if the user wants to send a private message
  else if (!strncmp(input, "/m", 2))
  {
    // initialize message
    message msg;
    msg.type = PRIVATE;

    char *specific_username, *chatMsg;

    specific_username = strtok(input + 3, " ");

    // check for errors
    if (specific_username == NULL)
    {
      printf(red "Private message usage: /m <username> <message>" defaultcolor);
      return;
    }

    if (strlen(specific_username) == 0)
    {
      printf(red "Username is needed for private message." defaultcolor);
      return;
    }

    if (strlen(specific_username) > 20)
    {
      printf(red "The username can't exceed 20 characters." defaultcolor);
      return;
    }

    chatMsg = strtok(NULL, "");

    if (chatMsg == NULL)
    {
      printf(red "Text at the end is needed for private message." defaultcolor);
      return;
    }

    strncpy(msg.user, specific_username, 20);
    strncpy(msg.text, chatMsg, 255);

    // send message to server
    if (send(connection->socket, &msg, sizeof(message), 0) == -1)
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
  //     printf(KRED "You must enter at least one username for a group message." RESET);
  //     return;
  //   }

  //   chatMsg = users[num_users - 1];

  //   if (strlen(chatMsg) == 0)
  //   {
  //     printf(KRED "You must enter a message to send to the specified users." RESET);
  //     return;
  //   }

  //   msg.group_size = num_users - 1;

  //   for (int i = 0; i < num_users - 1; i++)
  //   {
  //     strncpy(msg.group[i], users[i], 20);
  //   }

  //   strncpy(msg.data, chatMsg, 255);

  //   if (send(connection->socket, &msg, sizeof(message), 0) == -1)
  //   {
  //     perror("Send failed");
  //     exit(1);
  //   }

  //   free(users);
  // }
  else // regular public message
  {
    // initialize message
    message msg;
    msg.type = MESSAGE_TO_ALL;
    strncpy(msg.user, connection->username, 20);

    if (strlen(input) == 0)
    {
      return;
    }

    strncpy(msg.text, input, 255);

    // Send message to server
    if (send(connection->socket, &msg, sizeof(message), 0) == -1)
    {
      perror("Error: send");
      exit(1);
    }
  }
}

/**
 * @brief  handle the interaction with the server
 *
 * @param connection
 */
void server_interaction(info *connection)
{
  message msg;

  // Receive a reply from the server
  int recv_val = recv(connection->socket, &msg, sizeof(message), 0);
  if (recv_val == -1)
  {
    perror("recv failed");
    exit(1);
  }
  else if (recv_val == 0)
  {
    close(connection->socket);
    printf("Server closed.");
    exit(0);
  }

  switch (msg.type)
  {

  case CONNECTING:
    printf(yellow "%s is connected." defaultcolor "\n", msg.user);
    break;

  case DISCONNECTING:
    printf(yellow "%s has disconnected." defaultcolor "\n", msg.user);
    break;

  case USERS:
    printf("%s", msg.text);
    break;

  case MESSAGE_TO_ALL:
    printf(green "%s" defaultcolor ": %s\n", msg.user, msg.text);
    break;

  case PRIVATE:
    printf(white "From %s:" cyan " %s\n" defaultcolor, msg.user, msg.text);
    break;

    // case GROUP_MESSAGE:
    //   printf(white "From %s:" defaultcolor " %s\n" defaultcolor, msg.user, msg.text);
    //   break;

  case FULL:
    fprintf(stderr, red "Server can't accept new clients." defaultcolor "\n");
    exit(0);
    break;

  default:
    fprintf(stderr, red "Message type unknown." defaultcolor "\n");
    break;
  }
}

int main(int argc, char *argv[])
{
  // check if the user entered the correct number of arguments
  // connect to server
  info connection;
  fd_set file_descriptors;

  if (argc != 3)
  {
    fprintf(stderr, "Usage: %s <IP> <port>\n", argv[0]);
    exit(1);
  }

  connect_server(&connection, argv[1], argv[2]);

  FD_ZERO(&file_descriptors);
  FD_SET(STDIN_FILENO, &file_descriptors);
  FD_SET(connection.socket, &file_descriptors);
  fflush(stdin);

  while (1)
  {
    fd_set read_fds = file_descriptors;
    if (select(connection.socket + 1, &read_fds, NULL, NULL, NULL) < 0)
    {
      perror("Select failed.");
      exit(1);
    }

    // if there is something to read from stdin
    if (FD_ISSET(STDIN_FILENO, &read_fds))
    {
      user_input(&connection);
    }

    // if there is something to read from the server
    if (FD_ISSET(connection.socket, &read_fds))
    {
      server_interaction(&connection);
    }
  }

  // close the socket
  close(connection.socket);
  return 0;
}
