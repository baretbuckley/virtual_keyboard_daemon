#include <channel.h>

#include <sys/socket.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <stdlib.h>


struct ClientChannel {
    struct sockaddr_un address;
    int fd;
    unsigned char connected;
};

struct ServerChannel {
    struct sockaddr_un address;
    struct SerialMessage msgBuffer;
    int fd;
    int connFd; 
    unsigned char connected;
};



int open_af_unix_socket(struct sockaddr_un *address, const char* name) {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(fd == -1) {
        fprintf(stderr, "Failed to open AF_UNIX socket. ErrorNo=%d\n", errno);
        // Same as close channel
        close(fd);
        remove(address->sun_path);
        return fd;
    }
    address->sun_family = AF_UNIX;
    strcpy(address->sun_path, "/tmp/");
    strcpy(address->sun_path+(sizeof("/tmp/")-1), name);
    printf("name: %s, path: %s\n", name, address->sun_path);
    return fd;
}

struct ServerChannel *createChannel(const char *name) {
    struct ServerChannel *channel = (struct ServerChannel *)malloc(sizeof(struct ServerChannel));
    channel->fd = open_af_unix_socket(&(channel->address), name);
    
    if (bind(channel->fd, (struct sockaddr *)&(channel->address), sizeof(struct sockaddr))) {
        fprintf(stderr, "Failed to bind socket on Path=%s, Error: %s\n", channel->address.sun_path, strerror(errno));
        return NULL;
    }
    if (listen(channel->fd, 10)) {
        fprintf(stderr, "Error while accepting connection: %s\n", strerror(errno));
        return NULL;
    }
    channel->msgBuffer = initSerialMsgCapacity(CHANNEL_BUFFER_SIZE);
    channel->connected = 0;
    return channel;
}

void closeChannel(struct ServerChannel *channel) {
    close(channel->fd);
    unlink(channel->address.sun_path);
    remove(channel->address.sun_path);
}

int waitConnection(struct ServerChannel *channel) {
    if ((channel->connFd = accept(channel->fd, NULL, NULL)) == -1) {
        fprintf(stderr, "Error while accepting connection: %s\n", strerror(errno));
        closeChannel(channel);
        return -1;
    }
    channel->connected = 1;
    return 0;
}

struct SerialMessage *recieveMessage(struct ServerChannel *channel) {
    if (!channel->connected) {
        printf("Unable to recieve message, client not connected\n");
        return NULL;
    }
    
    int readCnt = read(channel->connFd, channel->msgBuffer.msgBuffer, channel->msgBuffer.capacity);
    channel->msgBuffer.msgLen = readCnt;
    printf("Read msg len %i\n", readCnt);
    if (readCnt <= 0) {
        if (readCnt == 0) {
            channel->connected = 0;
            clearRetainingCapacity(&(channel->msgBuffer));
            close(channel->connFd);
            return &(channel->msgBuffer);
        } else {
            fprintf(stderr, "Error while reading from channel: %s\n", strerror(errno));
            closeChannel(channel);
            return NULL;
        }
    }
    u_int32_t expectedLen = *((u_int32_t*)(channel->msgBuffer.msgBuffer));
    printf("Expected len is %i with curr msg len = %i, cap is %i\n", expectedLen, channel->msgBuffer.msgLen, channel->msgBuffer.capacity);
    if (expectedLen > channel->msgBuffer.capacity) {
        fprintf(stderr, "Warning revieved message larger than capacity\n");
    }
    while (channel->msgBuffer.msgLen < expectedLen) {
        int readCnt = read(channel->connFd, channel->msgBuffer.msgBuffer + channel->msgBuffer.msgLen, channel->msgBuffer.capacity);
        if (readCnt <= 0) {
            if (readCnt == 0) {
                fprintf(stderr, "Channel disconnected before end of message\n");
                closeChannel(channel);
                return NULL;
            } else {
                fprintf(stderr, "Error while reading from channel: %s\n", strerror(errno));
                closeChannel(channel);
                return NULL;
            }
        }
        channel->msgBuffer.msgLen += readCnt;
    }

    return &(channel->msgBuffer);

}

int isClientConnected(struct ServerChannel *channel) {
    return channel->connected;
}

void freeServerChannel(struct ServerChannel *channel) {
    deinitSerialMsg(&(channel->msgBuffer));
    free(channel);
}




struct ClientChannel *openChannel(const char *name) {
    struct ClientChannel *channel = (struct ClientChannel *)malloc(sizeof(struct ServerChannel));
    channel->fd = open_af_unix_socket(&(channel->address), name);

    if (connect(channel->fd, (struct sockaddr *)&(channel->address), sizeof(struct sockaddr))) {
        fprintf(stderr, "Failed to connect to socket on Path=%s, Error: %s\n", channel->address.sun_path, strerror(errno));
        disconnect(channel);
        return NULL;
    }
    channel->connected = 1;
    return channel;
}

void disconnect(struct ClientChannel *channel) {
    close(channel->fd);
    channel->connected = 0;
}

int sendMessage(struct ClientChannel *channel, struct SerialMessage msg) {
    if (!channel->connected) {
        printf("Unable to send message, channel not connected\n");
        return -1;
    }
    *((u_int32_t*)(msg.msgBuffer)) = msg.msgLen;
    printf("Message len is %i, %i\n", *((u_int32_t*)(msg.msgBuffer)), msg.msgLen);
    int written = write(channel->fd, msg.msgBuffer, msg.msgLen);
    printf("wrote %i bytes\n", written);
    if (written < 0) {
        fprintf(stderr, "Failed to write to file: %s\n", strerror(errno));
        return -1;
    }
    
    return written;
}

void freeClientChannel(struct ClientChannel *channel) {
    if (channel->connected)
        disconnect(channel);
    free(channel);
}