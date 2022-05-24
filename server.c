#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <pthread.h>

#include "tools.h"

#define MAX_CLIENTS 10

void start_server(info *server_info, int port)
{
    // create a socket
    if ((server_info->socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Error: creation socket");
        exit(1);
    }

    // set the socket address

    // adress family
    server_info->address.sin_family = AF_INET;
    // IPv4 adress of server (loopback)
    server_info->address.sin_addr.s_addr = htonl(INADDR_ANY);
    // port number
    server_info->address.sin_port = htons(port);

    // bind the socket to the address
    if (bind(server_info->socket, (struct sockaddr *)&server_info->address, sizeof(server_info->address)) < 0)
    {
        perror("Error: bind ");
        exit(1);
    }

    // listen for connections
    if (listen(server_info->socket, 5) < 0)
    {
        perror("Error: listen ");
        exit(1);
    }

    // Accept and incoming connection
    printf("Waiting for clients\n");
}

void send_public_message(info clients[], int sender, char *message_text)
{
    message msg;
    msg.type = PUBLIC_MESSAGE;
    strncpy(msg.username, clients[sender].username, 20);
    strncpy(msg.data, message_text, 256);
    int i = 0;
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        if (i != sender && clients[i].socket != 0)
        {
            if (send(clients[i].socket, &msg, sizeof(msg), 0) < 0)
            {
                perror("Send failed");
                exit(1);
            }
        }
    }
}

void send_private_message(info clients[], int sender,
                          char *username, char *message_text)
{
    message msg;
    msg.type = PRIVATE_MESSAGE;
    strncpy(msg.username, clients[sender].username, 20);
    strncpy(msg.data, message_text, 256);

    int i;
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        if (i != sender && clients[i].socket != 0 && strcmp(clients[i].username, username) == 0)
        {
            if (send(clients[i].socket, &msg, sizeof(msg), 0) < 0)
            {
                perror("Send failed");
                exit(1);
            }
            return;
        }
    }

    msg.type = USERNAME_ERROR;
    sprintf(msg.data, "Username \"%s\" does not exist or is not logged in.", username);

    if (send(clients[sender].socket, &msg, sizeof(msg), 0) < 0)
    {
        perror("Send failed");
        exit(1);
    }
}

void send_connect_message(info *clients, int sender)
{
    message msg;
    msg.type = CONNECT;
    strncpy(msg.username, clients[sender].username, 21);
    int i = 0;
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].socket != 0)
        {
            if (i == sender)
            {
                msg.type = SUCCESS;
                if (send(clients[i].socket, &msg, sizeof(msg), 0) < 0)
                {
                    perror("Send failed");
                    exit(1);
                }
            }
            else
            {
                if (send(clients[i].socket, &msg, sizeof(msg), 0) < 0)
                {
                    perror("Send failed");
                    exit(1);
                }
            }
        }
    }
}

void send_disconnect_message(info *clients, char *username)
{
    message msg;
    msg.type = DISCONNECT;
    strncpy(msg.username, username, 21);
    int i = 0;
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].socket != 0)
        {
            if (send(clients[i].socket, &msg, sizeof(msg), 0) < 0)
            {
                perror("Error: send");
                exit(1);
            }
        }
    }
}

void send_user_list(info *clients, int receiver)
{
    message msg;
    msg.type = GET_USERS;
    char *list = msg.data;

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].socket != 0)
        {
            list = stpcpy(list, clients[i].username);
            list = stpcpy(list, "\n");
        }
    }

    if (send(clients[receiver].socket, &msg, sizeof(msg), 0) < 0)
    {
        perror("Error: send");
        exit(1);
    }
}

void send_too_full_message(int socket)
{
    message too_full_message;
    too_full_message.type = TOO_FULL;

    if (send(socket, &too_full_message, sizeof(too_full_message), 0) < 0)
    {
        perror("Error: send");
        exit(1);
    }

    close(socket);
}

// close all the sockets then exit
void stop_server(info clients[])
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        close(clients[i].socket);
    }
    exit(0);
}

void client_message(info clients[], int id)
{
    message msg;

    if ((recv(clients[id].socket, &msg, sizeof(message), 0)) == 0)
    {
        printf(KRED "Client disconnected: %s.\n" RESET, clients[id].username);
        close(clients[id].socket);
        clients[id].socket = 0;
        send_disconnect_message(clients, clients[id].username);
    }
    else
    {

        switch (msg.type)
        {
        case GET_USERS:
            send_user_list(clients, id);
            break;

        case SET_USERNAME:;
            int i;
            for (i = 0; i < MAX_CLIENTS; i++)
            {
                if (clients[i].socket != 0 && strcmp(clients[i].username, msg.username) == 0)
                {
                    close(clients[id].socket);
                    clients[id].socket = 0;
                    return;
                }
            }

            strcpy(clients[id].username, msg.username);
            printf(KGRN "Client connected: %s\n" RESET, clients[id].username);
            send_connect_message(clients, id);
            break;

        case PUBLIC_MESSAGE:
            send_public_message(clients, id, msg.data);
            break;

        case PRIVATE_MESSAGE:
            send_private_message(clients, id, msg.username, msg.data);
            break;

        default:
            fprintf(stderr, "Unknown message type received.\n");
            break;
        }
    }
}

int get_max_fd(fd_set *set, info *server_info, info clients[])
{

    int max_fd = server_info->socket;
    int i;
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].socket > 0)
        {
            FD_SET(clients[i].socket, set);
            if (clients[i].socket > max_fd)
            {
                max_fd = clients[i].socket;
            }
        }
    }
    return max_fd;
}

void case_connection(info *server_info, info clients[])
{
    int new_socket;
    int address_len;
    new_socket = accept(server_info->socket, (struct sockaddr *)&server_info->address, (socklen_t *)&address_len);
    if (new_socket == -1)
    {
        perror("Error: accept");
        exit(1);
    }

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].socket == 0)
        {
            clients[i].socket = new_socket;
            break;
        }
        else if (i == MAX_CLIENTS - 1)
        {
            send_too_full_message(new_socket);
        }
    }
}

void case_user(info clients[])
{
    char input[255];
    fgets(input, sizeof(input), stdin);
    trim_newline(input);

    if (input[0] == 'q')
    {
        stop_server(clients);
    }
}

int main(int argc, char *argv[])
{
    // check if the port number is given
    if (argc != 2)
    {
        fprintf(stderr, KRED "Usage: %s <port>\n" RESET, argv[0]);
        exit(1);
    }

    printf(KGRN "Server started.\n" RESET);

    // initialize server and client info
    info server_info;
    info clients[MAX_CLIENTS];
    memset(clients, 0, sizeof(clients));

    // initialize server socket
    start_server(&server_info, atoi(argv[1]));

    // initialize fd_set
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    FD_SET(server_info.socket, &fds);

    while (1)
    {
        fd_set read_fds = fds;
        // get the max fd for select
        int max_fd = get_max_fd(&read_fds, &server_info, clients);

        // select on all sockets
        if (select(max_fd + 1, &read_fds, NULL, NULL, 0) == -1)
        {
            perror("Error: select");
            stop_server(clients);
        }

        // case: user input
        if (FD_ISSET(STDIN_FILENO, &read_fds))
        {
            case_user(clients);
        }

        // case: new connection
        if (FD_ISSET(server_info.socket, &read_fds))
        {
            case_connection(&server_info, clients);
        }

        // case: client message
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients[i].socket > 0 && FD_ISSET(clients[i].socket, &read_fds))
            {
                // client message
                client_message(clients, i);
            }
        }
    }

    return 0;
}
