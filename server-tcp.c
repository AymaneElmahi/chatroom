
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <pthread.h>
#include "tools.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CLTS_MAX 10

/**
 * @brief start the server and wait for connections.
 *
 * @param server_info  the server information
 * @param port       the port to listen on
 */
void server_start(info *server_info, int port)
{
    // create a socket
    if ((server_info->socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Error: creation socket");
        exit(1);
    }

    // adress family
    server_info->address.sin_family = AF_INET;
    // IPv4 adress of server (loopback)
    server_info->address.sin_addr.s_addr = htonl(INADDR_ANY);
    // port number
    server_info->address.sin_port = htons(port);

    // bind the socket to the address
    if (bind(server_info->socket, (struct sockaddr *)&server_info->address, sizeof(server_info->address)) == -1)
    {
        perror("Error: bind ");
        exit(1);
    }

    // listen for connections
    if (listen(server_info->socket, 5) == -1)
    {
        perror("Error: listen ");
        exit(1);
    }

    // Accept incoming connection
    printf("Waiting for clients\n");
}

/**
 * @brief  send a message to all clients
 *
 * @param clients  the clients
 * @param id    the id of the sender
 * @param text  the text to send
 */
void message_to_all(info clients[], int id, char *text)
{
    // initialize message
    message msg;
    msg.type = MESSAGE_TO_ALL;
    strncpy(msg.user, clients[id].username, 20);
    strncpy(msg.text, text, 256);

    // send message to all clients
    for (int i = 0; i < CLTS_MAX; i++)
    {
        if (i != id && clients[i].socket != 0)
        {
            if (send(clients[i].socket, &msg, sizeof(msg), 0) == -1)
            {
                perror("Error: send");
                exit(1);
            }
        }
    }
}

// void private_group_message(info clients[], int id, char **group, int group_size, char *text)
// {
//     message msg;
//     msg.type = GROUP_MESSAGE;
//     strncpy(msg.user, clients[id].username, 20);
//     strncpy(msg.text, text, 256);
//     // send to all clients in the group
//     for (int i = 0; i < group_size; i++)
//     {
//         int j = 0;
//         for (j = 0; j < CLTS_MAX; j++)
//         {
//             if (strcmp(clients[j].username, group[i]) == 0)
//             {
//                 if (send(clients[j].socket, &msg, sizeof(msg), 0) < 0)
//                 {
//                     perror("Error: send");
//                     exit(1);
//                 }
//             }
//         }
//     }
// }

/**
 * @brief  send a message to a specific client
 *
 * @param clients  the clients
 * @param id    the id of the sender
 * @param text  the text to send
 * @param target  the target client
 */
void private_message(info clients[], int id, char *user, char *text)
{
    // initialize message
    message msg;
    msg.type = PRIVATE;
    strncpy(msg.user, clients[id].username, 20);
    strncpy(msg.text, text, 256);

    // send message to specified user
    for (int i = 0; i < CLTS_MAX; i++)
    {
        if (i != id && clients[i].socket != 0 && strcmp(clients[i].username, user) == 0)
        {
            if (send(clients[i].socket, &msg, sizeof(msg), 0) < 0)
            {
                perror("Error: send");
                exit(1);
            }
            return;
        }
    }

    // send error message to client
    msg.type = ERROR_NAME;
    sprintf(msg.text, "Username \"%s\" inexistent.", user);

    if (send(clients[id].socket, &msg, sizeof(msg), 0) < 0)
    {
        perror("Error: send");
        exit(1);
    }
}

/**
 * @brief  handle when a client is connected
 *
 * @param clients  the clients
 * @param id  the id of the client
 */
void connecting_message(info *clients, int id)
{
    // initialize message
    message msg;
    msg.type = CONNECTING;
    strncpy(msg.user, clients[id].username, 21);

    // send message to all clients
    for (int i = 0; i < CLTS_MAX; i++)
    {
        if (clients[i].socket != 0)
        {
            if (i == id)
            {
                msg.type = SUCCESS;
                if (send(clients[i].socket, &msg, sizeof(msg), 0) < 0)
                {
                    perror("Error: send");
                    exit(1);
                }
            }
            else
            {
                if (send(clients[i].socket, &msg, sizeof(msg), 0) < 0)
                {
                    perror("Error: send");
                    exit(1);
                }
            }
        }
    }
}

/**
 * @brief  handle when a client is disconnected
 *
 * @param clients  the clients
 * @param name  the name of the client
 */
void disconnecting_message(info *clients, char *name)
{
    // initialize message
    message msg;
    msg.type = DISCONNECTING;
    strncpy(msg.user, name, 21);

    // send message to all clients
    for (int i = 0; i < CLTS_MAX; i++)
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

/**
 * @brief  handle when a client is typing /list
 *
 * @param clients  the clients
 * @param id  the id of the client typing /list
 */
void show_users(info *clients, int id)
{
    // initialize message
    message msg;
    msg.type = USERS;
    char *list = msg.text;

    // send message
    for (int i = 0; i < CLTS_MAX; i++)
    {
        if (clients[i].socket != 0)
        {
            list = stpcpy(list, clients[i].username);
            list = stpcpy(list, "\n");
        }
    }

    if (send(clients[id].socket, &msg, sizeof(msg), 0) < 0)
    {
        perror("Error: send");
        exit(1);
    }
}

/**
 * @brief  handle when more than CLTS_MAX clients are connected
 *
 * @param socket  the socket
 */
void full_message(int socket)
{
    // initialize message
    message full_message;
    full_message.type = FULL;

    // send message to client
    if (send(socket, &full_message, sizeof(full_message), 0) < 0)
    {
        perror("Error: send");
        exit(1);
    }

    close(socket);
}

/**
 * @brief  close the server
 *
 * @param clients  the clients
 */
void stop_server(info clients[])
{
    for (int i = 0; i < CLTS_MAX; i++)
    {
        close(clients[i].socket);
    }
    exit(0);
}

/**
 * @brief  handle when a message arrives in the server
 *
 * @param clients  the clients
 * @param id  the id of the sender
 */
void client_message(info clients[], int id)
{
    message msg;

    if ((recv(clients[id].socket, &msg, sizeof(message), 0)) == 0)
    {
        printf(red "Client disconnected: %s.\n" defaultcolor, clients[id].username);
        close(clients[id].socket);
        clients[id].socket = 0;
        disconnecting_message(clients, clients[id].username);
    }
    else
    {

        switch (msg.type)
        {
        case USERS:
            show_users(clients, id);
            break;

        case USERNAME:
            for (int i = 0; i < CLTS_MAX; i++)
            {
                if (clients[i].socket != 0 && strcmp(clients[i].username, msg.user) == 0)
                {
                    close(clients[id].socket);
                    clients[id].socket = 0;
                    return;
                }
            }

            strcpy(clients[id].username, msg.user);
            printf(green "Client connected: %s\n" defaultcolor, clients[id].username);
            connecting_message(clients, id);
            break;

        case MESSAGE_TO_ALL:
            message_to_all(clients, id, msg.text);
            break;

        case PRIVATE:
            private_message(clients, id, msg.user, msg.text);
            break;

            // case GROUP_MESSAGE:
            //     group_message(clients, id, msg.group, msg.group_size, msg.data);
            //     break;

        default:
            fprintf(stderr, "Unknown message type received.\n");
            break;
        }
    }
}

/**
 * @brief Get the max fd of the clients
 *
 * @param fds  the descriptors
 * @param server_info  the server info
 * @param clients  the clients
 * @return int  the max fd
 */
int get_max_fd(fd_set *fds, info *server_info, info clients[])
{
    int max_fd = server_info->socket;
    int i;
    for (i = 0; i < CLTS_MAX; i++)
    {
        if (clients[i].socket > 0)
        {
            FD_SET(clients[i].socket, fds);
            if (clients[i].socket > max_fd)
            {
                max_fd = clients[i].socket;
            }
        }
    }
    return max_fd;
}

/**
 * @brief  what to do when a client is connected
 *
 * @param server_info  the server info
 * @param clients  the clients
 */
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

    for (int i = 0; i < CLTS_MAX; i++)
    {
        if (clients[i].socket == 0)
        {
            clients[i].socket = new_socket;
            break;
        }
        else if (i == CLTS_MAX - 1)
        {
            full_message(new_socket);
        }
    }
}

void case_user(info clients[])
{
    char input[255];
    fgets(input, sizeof(input), stdin);
    delete_line(input);

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
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    printf("Server started...\n");

    // initialize server and client info
    info server_info;
    info clients[CLTS_MAX];
    memset(clients, 0, sizeof(clients));

    // initialize server socket
    server_start(&server_info, atoi(argv[1]));

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
        for (int i = 0; i < CLTS_MAX; i++)
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
