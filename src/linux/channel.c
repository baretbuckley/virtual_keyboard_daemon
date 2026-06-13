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
    unsigned char *msgBuffer;
    unsigned int msgCapacity;
    int fd;
    int connFd; 
    unsigned char connected;
    unsigned char bufferOwned; // If the memory in msgBuffer is owned and must be later freed
};

#define DEFAULT_PATH_PREFIX "/tmp/"

int open_af_unix_socket(struct sockaddr_un *address) {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(fd == -1) {
        fprintf(stderr, "Failed to open AF_UNIX socket: %s\n", strerror(errno));
        return fd;
    }
    address->sun_family = AF_UNIX;
    printf("path: %s\n", address->sun_path);

    return fd;
}

int open_server_unix_socket(struct sockaddr_un *address) {
    int fd = open_af_unix_socket(address);
    if (fd < 0) return fd;
    if (bind(fd, (struct sockaddr *)(address), sizeof(struct sockaddr))) {
        fprintf(stderr, "Failed to bind socket on Path=%s, Error: %s\n", address->sun_path, strerror(errno));
        close(fd);
        return -1;
    }
    if (listen(fd, 10)) {
        fprintf(stderr, "Error while preparing to accept connections: %s\n", strerror(errno));
        close(fd);
        unlink(address->sun_path);
        return -1;
    }
    return fd;
}

struct ServerChannel *createChannel(const char *name, unsigned char *buffer, unsigned int len) {
    struct ServerChannel *channel = (struct ServerChannel *)malloc(sizeof(struct ServerChannel));
    strcpy(channel->address.sun_path, DEFAULT_PATH_PREFIX);
    strcpy(channel->address.sun_path + (sizeof(DEFAULT_PATH_PREFIX)-1), name);
    
    channel->fd = open_server_unix_socket(&(channel->address));
    if (channel->fd < 0) {
        free(channel);
        return NULL;
    }
    
    
    if (len < 16) { // 4 byte len prefix + 12 bytes for the min needed capacity to fit any message type (assumes type and typeDelay can be split as needed)
        fprintf(stderr, "Error channel buffer too small, buffer capacity must be atleast 16 bytes\n");
        closeChannel(channel);
        free(channel);
    }
    if (buffer) {
        channel->msgBuffer = buffer;
        channel->bufferOwned = 0;
    } else {
        channel->msgBuffer = (unsigned char*)malloc(len);
        channel->bufferOwned = 1;
    }
    channel->msgCapacity = len;

    
    channel->connected = 0;
    return channel;
}

struct ServerChannel *createChannelWithPath(const char *path, unsigned char *buffer, unsigned int len) {
    struct ServerChannel *channel = (struct ServerChannel *)malloc(sizeof(struct ServerChannel));
    strcpy(channel->address.sun_path, path);

    channel->fd = open_server_unix_socket(&(channel->address));
    if (channel->fd < 0) {
        free(channel);
        return NULL;
    }
    
    if (len < 16) { // 4 byte len prefix + 12 bytes for the min needed capacity to fit any message type (assumes type and typeDelay can be split as needed)
        fprintf(stderr, "Error channel buffer too small, buffer capacity must be atleast 16 bytes\n");
        closeChannel(channel);
        free(channel);
    }
    if (buffer) {
        channel->msgBuffer = buffer;
        channel->bufferOwned = 0;
    } else {
        channel->msgBuffer = (unsigned char*)malloc(len);
        channel->bufferOwned = 1;
    }
    channel->msgCapacity = len;
    
    channel->connected = 0;
    return channel;
}

struct ServerChannel *createChannelWithFD(int fd, unsigned char *buffer, unsigned int len) {
    struct ServerChannel *channel = (struct ServerChannel *)malloc(sizeof(struct ServerChannel));


    channel->fd = fd;
    
    if (len < 16) { // 4 byte len prefix + 12 bytes for the min needed capacity to fit any message type (assumes type and typeDelay can be split as needed)
        fprintf(stderr, "Error channel buffer too small, buffer capacity must be atleast 16 bytes\n");
        closeChannel(channel);
        free(channel);
    }
    if (buffer) {
        channel->msgBuffer = buffer;
        channel->bufferOwned = 0;
    } else {
        channel->msgBuffer = (unsigned char*)malloc(len);
        channel->bufferOwned = 1;
    }
    channel->msgCapacity = len;
    
    channel->connected = 0;
    return channel;
}


void closeChannel(struct ServerChannel *channel) {
    close(channel->fd);
    unlink(channel->address.sun_path);
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

int recieveMessage(struct ServerChannel *channel, struct SerialMessage *msg) {
    if (!channel->connected) {
        printf("Unable to recieve message, client not connected\n");
        return -1;
    }
    unsigned int curLen = 0;
    int readCnt = read(channel->connFd, channel->msgBuffer, channel->msgCapacity);
    printf("Read %i bytes\n", readCnt);
    if (readCnt <= 0) {
        if (readCnt == 0) {
            channel->connected = 0;
            msg->msgLen = 0;
            close(channel->connFd);
            return 0;
        } else {
            fprintf(stderr, "Error while reading from channel: %s\n", strerror(errno));
            closeChannel(channel);
            return -1;
        }
    }
    curLen = readCnt;

    u_int32_t expectedLen = *((u_int32_t*)(channel->msgBuffer));
    printf("Expected len is %i with curr msg len = %i, cap is %i\n", expectedLen, curLen, channel->msgCapacity);
    if (expectedLen > channel->msgCapacity-4) {
        fprintf(stderr, "Warning revieved message larger than capacity\n");
    }
    while (curLen < expectedLen) {
        int readCnt = read(channel->connFd, channel->msgBuffer + curLen, channel->msgCapacity - curLen);
        if (readCnt <= 0) {
            if (readCnt == 0) {
                fprintf(stderr, "Channel disconnected before end of message\n");
                channel->connected = 0;
                closeChannel(channel);
                return -1;
            } else {
                fprintf(stderr, "Error while reading from channel: %s\n", strerror(errno));
                closeChannel(channel);
                return -1;
            }
        }
        curLen += readCnt;
    }
    msg->msgBuffer = channel->msgBuffer+4;
    msg->capacity = channel->msgCapacity-4;
    msg->msgLen = curLen-4;
    return 0;

}

int isClientConnected(struct ServerChannel *channel) {
    return channel->connected;
}

void freeServerChannel(struct ServerChannel *channel) {
    if (channel->bufferOwned)
        free(channel->msgBuffer);
    free(channel);
}



struct ClientChannel *openChannel(const char *name) {
    struct ClientChannel *channel = (struct ClientChannel *)malloc(sizeof(struct ServerChannel));
    strcpy(channel->address.sun_path, DEFAULT_PATH_PREFIX);
    strcpy(channel->address.sun_path + (sizeof(DEFAULT_PATH_PREFIX)-1), name);
    channel->fd = open_af_unix_socket(&(channel->address));

    if (connect(channel->fd, (struct sockaddr *)&(channel->address), sizeof(struct sockaddr)+1)) {
        fprintf(stderr, "Failed to connect to socket on Path=%s, Error: %s\n", channel->address.sun_path, strerror(errno));
        disconnect(channel);
        return NULL;
    }
    channel->connected = 1;
    return channel;
}

struct ClientChannel *openChannelWithPath(const char *path) {
    struct ClientChannel *channel = (struct ClientChannel *)malloc(sizeof(struct ServerChannel));
    strcpy(channel->address.sun_path, "/run/vkeyd.sock");
    channel->fd = open_af_unix_socket(&(channel->address));
    printf("Using path %s\n", channel->address.sun_path);

    // socklen_t len =
    //     offsetof(struct sockaddr_un, sun_path) +
    //     strlen(addr.sun_path) + 1;

// connect(fd, (struct sockaddr *)&addr, len);
    if (connect(channel->fd, (struct sockaddr *)&(channel->address), sizeof(struct sockaddr_un))) {
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

int sendMessage(struct ClientChannel *channel, unsigned char *msg, unsigned int len) {
    if (!channel->connected) {
        printf("Unable to send message, channel not connected\n");
        return -1;
    }
    printf("Message len is %i, %i\n", *((u_int32_t*)(msg)), len);
    int written = write(channel->fd, msg, len);
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