
#include <channel.h>
#include <virtual_keyboard.h>

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


int main() {
    // int bytesRead;

    signal(SIGINT, sig_int_close);

    struct SerialMessage *msgRef;


    keyboard = createKeyBoard();

    if (!keyboard) {
        printf("Failed to create virtual keyboard\n");
        return -1;
    }

    // Create channel
    channel = createChannel("vkeyd");


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
            while ((res = extractMessage(&extractor, &msg))) {
                if (res == -1) {
                    printf("Illformated message recieved\n");
                    break;
                }
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
                        fprintf(stderr, "Warning repeat command not currently supported\n");
                        break;
                    case M_ServerClose:
                        printf("Closing server\n");
                        cleanUp();
                        return 0;
                    default:
                        printf("Unrecognized Message type.");
                        break;
                    
                }
            }
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