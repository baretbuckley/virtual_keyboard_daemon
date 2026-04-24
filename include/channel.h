#ifndef CHANNEL_H
#define CHANNEL_H

#include "message.h"

struct ClientChannel;
struct ServerChannel;

// Server Side API



struct ServerChannel *createChannel(const char *name);

void closeChannel(struct ServerChannel *channel);

int waitConnection(struct ServerChannel *channel);

struct SerialMessage *recieveMessage(struct ServerChannel *channel);

int isClientConnected(struct ServerChannel *channel);

void freeServerChannel(struct ServerChannel *channel);



struct ClientChannel *openChannel(const char *name);

void disconnect(struct ClientChannel *channel);

int sendMessage(struct ClientChannel *channel, struct SerialMessage msg);

void freeClientChannel(struct ClientChannel *channel);

#endif // CHANNEL_H