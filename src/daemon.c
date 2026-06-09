
#include <channel.h>
#include <virtual_keyboard.h>
#include <daemon_options.h>

// #include <windows.h>
// #include <winsock2.h>
// #include <afunix.h>
// #include <ws2tcpip.h>
// #include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

// #pragma comment(lib, "Ws2_32.lib")



static struct KeyBoard *keyboard = NULL;
static struct ServerChannel *channel = NULL;

void cleanUp() {
    if (channel) {
        printf("Cleaning channel\n");
        closeChannel(channel);
        freeServerChannel(channel);
    }
    if (keyboard) {
        deleteKeyBoard(keyboard);
    }
}

void sig_int_close(int sig){
    cleanUp();
    exit(-1);
}


enum ExtractResult {
    ER_Success = 1,
    ER_EndOfInput = 0,
    ER_Error = -1,
    ER_CloseServer = -2,
};

void skipMsg(struct SerialExtractor *extractor) {
    struct Message msg;
    int res = extractMessage(extractor, &msg);
    if (res <= 0) return;
    if (msg.type == M_RepeatNext) {
        skipMsg(extractor);
    }
}

enum ExtractResult executeMsg(struct SerialExtractor *extractor) {
    struct Message msg;
    int res;
    res = extractMessage(extractor, &msg);
    if (res <= 0) {
        if (res == 0) return ER_EndOfInput;
        else return ER_Error;
    }
    printf("Extracted message\n");
    switch (msg.type) {
        case M_Type:
            typeString(keyboard, msg.msg.str, 0);
            break;
        case M_TypeDelay:
            typeString(keyboard, msg.msg.str, msg.delay);
            break;
        case M_Press:
            tapKey(keyboard, msg.msg.key);
            break;
        case M_Hold:
            pressKey(keyboard, msg.msg.key);
            break;
        case M_Release:
            releaseKey(keyboard, msg.msg.key);
            break;
        case M_PressFor:
            printf("Pressing %s with a delay of %i\n", keycodeAsString(msg.msg.key), msg.delay);
            pressKey(keyboard, msg.msg.key);
            usleep(msg.delay*1000);
            releaseKey(keyboard, msg.msg.key);
            break;
        case M_Delay:
            usleep(msg.delay*1000);
            break;
        case M_RepeatNext:
            struct SerialExtractor extractorCopy = *extractor;
            printf("count is %i\n", msg.msg.count);
            for (unsigned int i = 0; i < msg.msg.count; i++) {
                printf("Repeating\n");
                res = executeMsg(&extractorCopy);
                extractorCopy = *extractor; // Prevent extractor from creeping forwards
                if (res <= 0) return -1;
                if (msg.delay != 0) usleep(msg.delay*1000);
            }
            skipMsg(extractor);
            break;
        case M_ServerClose:
            return ER_CloseServer;
        default:
            printf("Unrecognized Message type.");
            return ER_Error;
        
    }

    return ER_Success;

}


int main(int argc, const char **argv) {
    // int bytesRead;


    // Parse vkey options
    struct DaemonOptContext context;
    int read;
    if (parseDaemonOptions((const char**)argv, &context, &read)) {
        // If failed to parse, just imediatly terminate
        // any errors are reported in the parseClientOptions function
        return -1;
    }
    // Besides error checking, further steps in working with the client context is put off
    // until opening the channel after processing all subcommands and constructing the message
    if (context.channelName && context.channelPath) {
        fprintf(stderr, "%s: setting both the --channel-name and --linux-channel-path options is not supported\n", argv[0]);
        fprintf(stderr, "Try 'vkey --help' for more information.\n");
        return -1;
    }
#ifndef __linux__
    if (context.channelPath) {
        fprintf(stderr, "%s: option --linux-channel-path is not available for operating systems other than linux\n", argv[0]);
    }
#endif



    signal(SIGINT, sig_int_close);

    struct SerialMessage *msgRef;


    keyboard = createKeyBoard();

    if (!keyboard) {
        printf("Failed to create virtual keyboard\n");
        return -1;
    }

    // Construct channel based on result of client options
    // channelPath (on linux) ->     <channelPath>
    // channelName ->                <DEFAULT PREFIX>/<channelName>
    // neither ->                    <DEFAULT PREFIX>/vkeyd
    // Only check if channelPath is provided if it is a linux system, otherwise just check channelName
#ifdef __linux__
    if (context.channelPath != 0) {
        printf("Using channel path\n");
        channel = createChannelWithPath(context.channelPath);
    }
    else
#endif
    if (context.channelName != 0) {
        printf("Using channel name\n");
        channel = createChannel(context.channelName);
    } else {
        printf("Using default name\n");
        channel = createChannel("vkeyd");
    }


    if (!channel) {
        printf("Failed to create channel\n");
        return -1;
    }

    while (1) {

        // Wait untill client connects 
        printf("Connecting\n");
        if (waitConnection(channel)) {
            printf("Failed to connect to client\n");
            cleanUp();
            return -1;
        }


        // While client is connects keep reading from the channel
        // If recieved "exit" shut down server
        while (isClientConnected(channel)) {
            printf("Recieving message\n");
            msgRef = recieveMessage(channel);
            printf("reading message\n");
            // bytesRead = recieveMessage(channel, buffer, BUFFER_SIZE);
            
            if (msgRef == NULL) {
                printf("Failed to read from pipe.");
                cleanUp();
                return -1;
            }
            struct SerialExtractor extractor = serialExtractor(msgRef);
            struct Message msg;
            int res;
            while ((res = executeMsg(&extractor)) == ER_Success);
            switch (res) {
                case ER_CloseServer:
                    printf("Closing server\n");
                    releaseAllKeys(keyboard);
                    cleanUp();
                    return 0;
                case ER_EndOfInput:
                    break;
                case ER_Error:
                    printf("Illformated message recieved\n");
                    break;
            }
            // while ((res = extractMessage(&extractor, &msg))) {
            //     if (res == -1) {
            //         printf("Illformated message recieved\n");
            //         break;
            //     }
            //     switch (msg.type) {
            //         case M_Type:
            //             typeString(keyboard, msg.msg.str, 0);
            //             break;
            //         case M_TypeDelay:
            //             typeString(keyboard, msg.msg.str, msg.delay);
            //             break;
            //         case M_Press:
            //             tapKey(keyboard, msg.msg.key);
            //             break;
            //         case M_Hold:
            //             pressKey(keyboard, msg.msg.key);
            //             break;
            //         case M_Release:
            //             releaseKey(keyboard, msg.msg.key);
            //             break;
            //         case M_PressFor:
            //             printf("Pressing %s with a delay of %i\n", keycodeAsString(msg.msg.key), msg.delay);
            //             pressKey(keyboard, msg.msg.key);
            //             usleep(msg.delay*1000);
            //             releaseKey(keyboard, msg.msg.key);
            //             break;
            //         case M_Delay:
            //             usleep(msg.delay*1000);
            //             break;
            //         case M_RepeatNext:
            //             fprintf(stderr, "Warning repeat command not currently supported\n");
            //             break;
            //         case M_ServerClose:
            //             printf("Closing server\n");
            //             cleanUp();
            //             return 0;
            //         default:
            //             printf("Unrecognized Message type.");
            //             break;
                    
            //     }
            // }
            releaseAllKeys(keyboard);

            printf("serial message length: %i\n", msgRef->msgLen);
            printSerialMsg(*msgRef);


            // if (!strcmp(buffer, "exit")) {
            //     printf("Closing server\n");
            //     closeChannel(channel);
            //     freeServerChannel(channel);
            //     return 0;
            // }
            // printf("%s\n", buffer);
        }

    }


  }
// #include <sys/


// int openSocket(char *socket_path) {

//     if (socket_path)
// },kjgjji lp/;lki