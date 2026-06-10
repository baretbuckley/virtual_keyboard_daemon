#ifndef CHANNEL_H
#define CHANNEL_H

#include "message.h"

struct ClientChannel;
struct ServerChannel;

// Defined here for shared access between backends
#define CHANNEL_BUFFER_SIZE 2048



// Server Side API

struct ServerChannel *createChannel(const char *name, unsigned char *buffer, unsigned int len);

#ifdef __linux__
struct ServerChannel *createChannelWithPath(const char *path, unsigned char *buffer, unsigned int len);

struct ServerChannel *createChannelWithFD(int fd, unsigned char *buffer, unsigned int len);
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

struct ClientChannel *openChannel(const char *name);

#ifdef __linux__
struct ClientChannel *openChannelWithPath(const char *path);
#endif

void disconnect(struct ClientChannel *channel);

int sendMessage(struct ClientChannel *channel, unsigned char *msg, unsigned int len);

void freeClientChannel(struct ClientChannel *channel);

#endif // CHANNEL_H