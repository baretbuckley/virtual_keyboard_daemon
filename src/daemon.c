
#include <channel.h>
#include <virtual_keyboard.h>

// #include <windows.h>
// #include <winsock2.h>
// #include <afunix.h>
// #include <ws2tcpip.h>
// #include <iphlpapi.h>
#include <stdio.h>


// #pragma comment(lib, "Ws2_32.lib")

int main() {
    // int bytesRead;
    struct SerialMessage *msgRef;

    struct KeyBoard *keyboard = createKeyBoard();

    // Create channel
    struct ServerChannel *channel = createChannel("vkeyd5");


    if (!channel) {
        printf("Failed to create channel\n");
        freeServerChannel(channel);
        return -1;
    }

    while (1) {

        // Wait untill client connects 
        printf("Connecting\n");
        if (waitConnection(channel)) {
            printf("Failed to connect to client\n");
            freeServerChannel(channel);
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
                freeServerChannel(channel);
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
                    case M_Key: tapKey(keyboard, msg.msg.key); break;
                    case M_String: typeString(keyboard, msg.msg.str); break;
                    case M_ServerClose:
                        printf("Closing server\n");
                        closeChannel(channel);
                        freeServerChannel(channel);
                        deleteKeyBoard(keyboard);
                        return 0;
                    default:
                        printf("Unrecognized Message type.");
                        break;
                    
                }
            }

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