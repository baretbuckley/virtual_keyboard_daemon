#ifndef CHANNEL_H
#define CHANNEL_H

#include "message.h"

#ifdef __linux__
#include <sys/socket.h>
#include <sys/un.h>

#elif defined(_WIN32) || defined(_WIN64)

#endif

struct ClientChannel {
#ifdef __linux__
    struct sockaddr_un address;
    int fd;
    unsigned char connected;
#elif defined(_WIN32) || defined(_WIN64)
    
#endif
};
struct ServerChannel{
#ifdef __linux__
    struct sockaddr_un address;
    unsigned char *msgBuffer;
    unsigned int msgCapacity;
    int fd;
    int connFd; 
    unsigned char connected;
    unsigned char bufferOwned; // If the memory in msgBuffer is owned and must be later freed
#elif defined(_WIN32) || defined(_WIN64)

#endif
};

// Defined here for shared access between backends
#define CHANNEL_BUFFER_SIZE 2048



// Server Side API

int createChannel(struct ServerChannel *handle, const char *name, unsigned char *buffer, unsigned int len);

#ifdef __linux__
int createChannelWithPath(struct ServerChannel *handle, const char *path, unsigned char *buffer, unsigned int len);

int createChannelWithFD(struct ServerChannel *handle, int fd, unsigned char *buffer, unsigned int len);
#endif

void closeChannel(struct ServerChannel *channel);

int waitConnection(struct ServerChannel *channel);

// Recieve message and store reference in msg
// Returns 0 on success, -1 on error
// Contents will become undefined with any future call to recieveMessage
// Contents of msg will also be undefined after error
// On end of input, msg will have a message length of 0 and set channel->connected to false
int recieveMessage(struct ServerChannel *channel, struct SerialMessage *msg);

int isClientConnected(struct ServerChannel *channel);

void freeServerChannel(struct ServerChannel *channel);



// Client Side API

int openChannel(struct ClientChannel *handle, const char *name);

#ifdef __linux__
int openChannelWithPath(struct ClientChannel *handle, const char *path);
#endif

void disconnect(struct ClientChannel *channel);

int sendMessage(struct ClientChannel *channel, unsigned char *msg, unsigned int len);

void freeClientChannel(struct ClientChannel *channel);

#endif // CHANNEL_H