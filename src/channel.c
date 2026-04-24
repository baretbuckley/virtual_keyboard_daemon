#include <channel.h>

#include <windows.h>
#include <stdio.h>

struct ServerChannel {
    char *pipePath;
    struct SerialMessage msgBuffer;
    HANDLE pipe;
    BOOL connected;
};

struct ClientChannel {
    char *pipePath;
    HANDLE pipe;
    BOOL connected;
};

int reopenChannel(struct ServerChannel *channel) {
    channel->connected = FALSE;
    channel->pipe = CreateNamedPipe(
        channel->pipePath,
        PIPE_ACCESS_INBOUND,
        PIPE_TYPE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        2048,
        2048,
        0,
        NULL
    );
    if (channel->pipe == INVALID_HANDLE_VALUE) {
        return -1;
    }
    return 0;
}

struct ServerChannel *createChannel(const char *name) {
    struct ServerChannel *server = malloc(sizeof(struct ServerChannel));

    server->msgBuffer = initSerialMsgCapacity(1024);

    char *pipePath = malloc(256);
    sprintf(pipePath, "\\\\.\\pipe\\%s", name);
    server->pipePath = pipePath;

    if (reopenChannel(server) == -1) {
        return NULL;
    } else {
        return server;
    }

    // server->connected = FALSE;
    // server->pipe = CreateNamedPipe(
    //     pipePath,
    //     PIPE_ACCESS_INBOUND,
    //     PIPE_TYPE_MESSAGE | PIPE_WAIT,
    //     PIPE_UNLIMITED_INSTANCES,
    //     2048,
    //     2048,
    //     0,
    //     NULL
    // );
    // if (server->pipe == INVALID_HANDLE_VALUE) {
    //     printf("Failed to created pipe");
    //     return -1;
    // }
    // return server;
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


struct SerialMessage *recieveMessage(struct ServerChannel *channel) {
    if (!channel->connected) {
        printf("Unable to recieve message, client not connected\n");
        return NULL;
    }
    long unsigned int read;

    if (!ReadFile(channel->pipe, channel->msgBuffer.msgBuffer, channel->msgBuffer.capacity, &read, NULL) || read == 0) {
        if (GetLastError() == ERROR_BROKEN_PIPE) {
            channel->connected = FALSE;
            reopenChannel(channel);
            clearRetainingCapacity(&(channel->msgBuffer));
            return &(channel->msgBuffer);
        } else {
            printf("Failed to read from pipe. Error code %li\n", GetLastError());
            return NULL;
        }
    }
    if (read == 0) return NULL;
    channel->msgBuffer.msgLen = read-4; // -4 to pop off the null terminal from the length
    return &(channel->msgBuffer);

}

int isClientConnected(struct ServerChannel *channel) {
    return channel->connected;
}

void freeServerChannel(struct ServerChannel *channel) {
    free(channel->pipePath);
    deinitSerialMsg(&(channel->msgBuffer));
}




struct ClientChannel *openChannel(const char *name) {
    struct ClientChannel *client = malloc(sizeof(struct ClientChannel));

    char *pipePath = malloc(256);
    sprintf(pipePath, "\\\\.\\pipe\\%s", name);
    client->pipePath = pipePath;

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

int sendMessage(struct ClientChannel *channel, struct SerialMessage msg) {
    if (!channel->connected) {
        printf("Unable to send message, channel not connected\n");
        return -1;
    }
    

    unsigned long written;
    if (!WriteFile(channel->pipe, msg.msgBuffer, serialFullLen(msg), &written, NULL)) {
        printf("Failed to write to file, error: %i \n", GetLastError());
        return -1;
    }
    return written;
    return 1;
}

void freeClientChannel(struct ClientChannel *channel) {
    if (channel->connected)
        disconnect(channel);
    free(channel->pipePath);
}