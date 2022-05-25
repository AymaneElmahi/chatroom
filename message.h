#ifndef MESSSAGE_H
#define MESSSAGE_H

// messages
typedef enum
{
    CONNECTING,
    DISCONNECTING,
    USERS,
    USERNAME,
    MESSAGE_TO_ALL,
    PRIVATE,
    FULL,
    ERROR_NAME,
    SUCCESS,
    ERROR
    // GROUP_MESSAGE

} msg_type;

typedef struct
{
    msg_type type;
    char user[21];
    char text[256];
    // char **group;      /*for private_group_message*/
    // int group_size;    /*for private_group_message

} message;

#endif