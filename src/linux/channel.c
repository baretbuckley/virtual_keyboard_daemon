#include <channel.h>

#include <sys/socket.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <stdlib.h>


// struct ClientChannel {
//     struct sockaddr_un address;
//     int fd;
//     unsigned char connected;
// };

// struct ServerChannel {
//     struct sockaddr_un address;
//     unsigned char *msgBuffer;
//     unsigned int msgCapacity;
//     int fd;
//     int connFd; 
//     unsigned char connected;
//     unsigned char bufferOwned; // If the memory in msgBuffer is owned and must be later freed
// };

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

int createChannel(struct ServerChannel *handle, const char *name, unsigned char *buffer, unsigned int len) {
    memset(handle, 0, sizeof(struct ServerChannel));

    strcpy(handle->address.sun_path, DEFAULT_PATH_PREFIX);
    strcpy(handle->address.sun_path + (sizeof(DEFAULT_PATH_PREFIX)-1), name);
    
    handle->fd = open_server_unix_socket(&(handle->address));
    if (handle->fd < 0) {
        return -1;
    }
    
    
    if (len < 16) { // 4 byte len prefix + 12 bytes for the min needed capacity to fit any message type (assumes type and typeDelay can be split as needed)
        fprintf(stderr, "Error channel buffer too small, buffer capacity must be atleast 16 bytes\n");
        closeChannel(handle);
        return -1;
    }
    if (buffer) {
        handle->msgBuffer = buffer;
        handle->bufferOwned = 0;
    } else {
        handle->msgBuffer = (unsigned char*)malloc(len);
        handle->bufferOwned = 1;
    }
    handle->msgCapacity = len;

    
    handle->connected = 0;
    return 0;
}

int createChannelWithPath(struct ServerChannel *handle, const char *path, unsigned char *buffer, unsigned int len) {
    memset(handle, 0, sizeof(struct ServerChannel));
    
    strcpy(handle->address.sun_path, path);

    handle->fd = open_server_unix_socket(&(handle->address));
    if (handle->fd < 0) {
        return -1;
    }
    
    if (len < 16) { // 4 byte len prefix + 12 bytes for the min needed capacity to fit any message type (assumes type and typeDelay can be split as needed)
        fprintf(stderr, "Error channel buffer too small, buffer capacity must be atleast 16 bytes\n");
        closeChannel(handle);
        return -1;
    }
    if (buffer) {
        handle->msgBuffer = buffer;
        handle->bufferOwned = 0;
    } else {
        handle->msgBuffer = (unsigned char*)malloc(len);
        handle->bufferOwned = 1;
    }
    handle->msgCapacity = len;
    
    handle->connected = 0;
    return 0;
}

int createChannelWithFD(struct ServerChannel *handle, int fd, unsigned char *buffer, unsigned int len) {
    memset(handle, 0, sizeof(struct ServerChannel));


    handle->fd = fd;
    
    if (len < 16) { // 4 byte len prefix + 12 bytes for the min needed capacity to fit any message type (assumes type and typeDelay can be split as needed)
        fprintf(stderr, "Error channel buffer too small, buffer capacity must be atleast 16 bytes\n");
        closeChannel(handle);
        return -1;
    }
    if (buffer) {
        handle->msgBuffer = buffer;
        handle->bufferOwned = 0;
    } else {
        handle->msgBuffer = (unsigned char*)malloc(len);
        handle->bufferOwned = 1;
    }
    handle->msgCapacity = len;
    
    handle->connected = 0;
    return 0;
}


void closeChannel(struct ServerChannel *channel) {
    close(channel->fd);
    unlink(channel->address.sun_path);
    if (channel->bufferOwned && channel->msgBuffer) {
        free(channel->msgBuffer);
        channel->msgBuffer = NULL;
    }
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



int openChannel(struct ClientChannel *handle, const char *name) {
    memset(handle, 0, sizeof(struct ClientChannel));
    strcpy(handle->address.sun_path, DEFAULT_PATH_PREFIX);
    strcpy(handle->address.sun_path + (sizeof(DEFAULT_PATH_PREFIX)-1), name);
    handle->fd = open_af_unix_socket(&(handle->address));

    if (connect(handle->fd, (struct sockaddr *)&(handle->address), sizeof(struct sockaddr)+1)) {
        fprintf(stderr, "Failed to connect to socket on Path=%s, Error: %s\n", handle->address.sun_path, strerror(errno));
        disconnect(handle);
        return -1;
    }
    handle->connected = 1;
    return 0;
}

int openChannelWithPath(struct ClientChannel *handle, const char *path) {
    memset(handle, 0, sizeof(struct ServerChannel));
    strcpy(handle->address.sun_path, "/run/vkeyd.sock");
    handle->fd = open_af_unix_socket(&(handle->address));
    printf("Using path %s\n", handle->address.sun_path);

    // socklen_t len =
    //     offsetof(struct sockaddr_un, sun_path) +
    //     strlen(addr.sun_path) + 1;

// connect(fd, (struct sockaddr *)&addr, len);
    if (connect(handle->fd, (struct sockaddr *)&(handle->address), sizeof(struct sockaddr_un))) {
        fprintf(stderr, "Failed to connect to socket on Path=%s, Error: %s\n", handle->address.sun_path, strerror(errno));
        disconnect(handle);
        return -1;
    }
    handle->connected = 1;
    return 0;
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