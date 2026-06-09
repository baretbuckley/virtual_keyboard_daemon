#ifndef CHANNEL_H
#define CHANNEL_H

#include "message.h"

struct ClientChannel;
struct ServerChannel;

// Defined here for shared access between backends
#define CHANNEL_BUFFER_SIZE 2048



// Server Side API

struct ServerChannel *createChannel(const char *name);

#ifdef __linux__
struct ServerChannel *createChannelWithPath(const char *path);
#endif

void closeChannel(struct ServerChannel *channel);

int waitConnection(struct ServerChannel *channel);

struct SerialMessage *recieveMessage(struct ServerChannel *channel);

int isClientConnected(struct ServerChannel *channel);

void freeServerChannel(struct ServerChannel *channel);



// Client Side API

struct ClientChannel *openChannel(const char *name);

#ifdef __linux__
struct ClientChannel *openChannelWithPath(const char *path);
#endif

void disconnect(struct ClientChannel *channel);

int sendMessage(struct ClientChannel *channel, struct SerialMessage msg);

void freeClientChannel(struct ClientChannel *channel);

#endif // CHANNEL_H