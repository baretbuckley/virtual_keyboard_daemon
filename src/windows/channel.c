#include <channel.h>

#include <windows.h>
#include <stdio.h>


struct ServerChannel {
    unsigned char *msgBuffer;
    unsigned int msgCapacity;
    HANDLE pipe;
    BOOL connected;
    BOOL bufferOwned; // If the memory in msgBuffer is owned and must be later freed
    char pipePath[256];
};

struct ClientChannel {
    HANDLE pipe;
    char pipePath[256];
    BOOL connected;
};

int reopenChannel(struct ServerChannel *channel) {
    channel->connected = FALSE;
    channel->pipe = CreateNamedPipe(
        channel->pipePath,
        PIPE_ACCESS_INBOUND,
        PIPE_TYPE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        CHANNEL_BUFFER_SIZE,
        CHANNEL_BUFFER_SIZE,
        0,
        NULL
    );
    if (channel->pipe == INVALID_HANDLE_VALUE) {
        return -1;
    }
    return 0;
}

struct ServerChannel *createChannel(const char *name, unsigned char *buffer, unsigned int len) {
    struct ServerChannel *server = malloc(sizeof(struct ServerChannel));

    sprintf(&server->pipePath, "\\\\.\\pipe\\%s", name);

    if (reopenChannel(server) == -1) {
        free(server);
        return NULL;
    }

    if (buffer) {
        server->msgBuffer = buffer;
        server->bufferOwned = 0;
    } else {
        server->msgBuffer = (unsigned char*)malloc(len);
        server->bufferOwned = 1;
    }
    server->msgCapacity = len;

    return server;
}

int waitConnection(struct ServerChannel *channel) {
    BOOL fConnected = ConnectNamedPipe(channel->pipe, NULL) ? 
        TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

    if (!fConnected) {
        printf("ConnectNamedPipe failed with code %li\n", GetLastError());
        CloseHandle(channel->pipe);
        return -1;
    }
    channel->connected = TRUE;
    return 0;
}

void closeChannel(struct ServerChannel *channel) {
    CloseHandle(channel->pipe);
}


int recieveMessage(struct ServerChannel *channel, struct SerialMessage *msg) {
    // if (!channel->connected) {
    //     printf("Unable to recieve message, client not connected\n");
    //     return -1;
    // }
    // long unsigned int read;

    // if (!ReadFile(channel->pipe, channel->msgBuffer, channel->msgCapacity, &read, NULL) || read == 0) {
    //     if (GetLastError() == ERROR_BROKEN_PIPE) {
    //         channel->connected = FALSE;
    //         reopenChannel(channel);
    //         clearRetainingCapacity(&(channel->msgBuffer));
    //         return &(channel->msgBuffer);
    //     } else {
    //         printf("Failed to read from pipe. Error code %li\n", GetLastError());
    //         closeChannel(channel);
    //         return -1;
    //     }
    // }
    // if (read == 0) return -1;
    // channel->msgBuffer.msgLen = read-4; // -4 to pop off the null terminal from the length
    // return &(channel->msgBuffer);



    if (!channel->connected) {
        printf("Unable to recieve message, client not connected\n");
        return -1;
    }
    unsigned int curLen = 0;
    unsigned int readCnt;
    if (!ReadFile(channel->pipe, channel->msgBuffer, channel->msgCapacity, &readCnt, NULL) || readCnt == 0) {
        if (GetLastError() == ERROR_BROKEN_PIPE) {
            channel->connected = FALSE;
            reopenChannel(channel);
            msg->msgLen = 0;
            return 0;
        } else {
            printf("Failed to read from pipe. Error code %li\n", GetLastError());
            closeChannel(channel);
            return -1;
        }
    }
    printf("Finished read\n");
    curLen = readCnt;

    uint32_t expectedLen = *((uint32_t*)(channel->msgBuffer));
    printf("Expected len is %i with curr msg len = %i, cap is %i\n", expectedLen, curLen, channel->msgCapacity);
    if (expectedLen > channel->msgCapacity-4) {
        fprintf(stderr, "Warning revieved message larger than capacity\n");
    }
    while (curLen < expectedLen) {
        if (!ReadFile(channel->pipe, channel->msgBuffer + curLen, channel->msgCapacity - curLen, &readCnt, NULL) || readCnt == 0) {
            if (GetLastError() == ERROR_BROKEN_PIPE) {
                fprintf(stderr, "Channel disconnected before end of message\n");
                channel->connected = FALSE;
                closeChannel(channel);
                return -1;
            } else {
                fprintf(stderr, "Failed to read from pipe. Error code %li\n", GetLastError());
                closeChannel(channel);
                return -1;
            }
        }
        printf("Finished read\n");
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
    if (channel->connected)
        closeChannel(channel);
    if (channel->bufferOwned)
        free(channel->msgBuffer);
    free(channel);
}





struct ClientChannel *openChannel(const char *name) {
    struct ClientChannel *client = malloc(sizeof(struct ClientChannel));

    sprintf(client->pipePath, "\\\\.\\pipe\\%s", name);

    client->pipe = CreateFile ( 
        client->pipePath,   // pipe name  
        GENERIC_WRITE, 
        0,              // no sharing 
        NULL,           // default security attributes
        OPEN_EXISTING,  // opens existing pipe 
        0,              // default attributes 
        NULL);          // no template file 

    if (client->pipe == INVALID_HANDLE_VALUE) {
        printf("Failed to open pipe, error: %li\n", GetLastError());
        return NULL;
    }
    client->connected = TRUE;
    return client;
}

void disconnect(struct ClientChannel *channel) {
    CloseHandle(channel->pipe);
    channel->connected = FALSE;
}

int sendMessage(struct ClientChannel *channel, unsigned char *msg, unsigned int len) {
    if (!channel->connected) {
        printf("Unable to send message, channel not connected\n");
        return -1;
    }
    
    printf("Message len is %i, %i\n", *((uint32_t*)(msg)), len);
    unsigned long written;
    if (!WriteFile(channel->pipe, msg, len, &written, NULL)) {
        printf("Failed to write to file, error: %li \n", GetLastError());
        return -1;
    }
    
    return written;
}

void freeClientChannel(struct ClientChannel *channel) {
    if (channel->connected)
        disconnect(channel);
    free(channel);
}