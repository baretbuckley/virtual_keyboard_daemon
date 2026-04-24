#include <channel.h>
#include <message.h>

#include <stdio.h>
#include <windows.h>

int main(int argc, char** argv) {

    struct SerialMessage smsg = initSerialMsg();
    // serialMsgAppendKey(&smsg, K_A);
    // serialMsgAppendKey(&smsg, K_C);
    // serialMsgAppendString(&smsg, "This is a string to be sent");
    

    struct ClientChannel *channel = openChannel("vkeyd");

    if (channel == NULL) {
        printf("Failed to open channel\n");
        freeClientChannel(channel);
        return -1;
    }

    long int written = 0;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "close")) {
            serialMsgAppendServerClose(&smsg);
        } else if (!strcmp(argv[i], "type")) {
            ++i;
            serialMsgAppendString(&smsg, argv[i]);
        } else if (!strcmp(argv[i], "press")) {
            ++i;
            if ((argv[i][0] >= 'A' && argv[i][0] <= 'Z') || (argv[i][0] >= '0' && argv[i][0] <= '9') ) {
                serialMsgAppendKey(&smsg, argv[i][0]);
            } else {
                printf("Invaild key %s\n", argv[i]);
            }
        }
        
    }
    

    if ((written = sendMessage(channel, smsg)) < 0) {
        printf("Failed to write to file\n");
        freeClientChannel(channel);
        return -1;
    }
    disconnect(channel);

    printf("Sent message:\n");
    printSerialMsg(smsg);

    freeClientChannel(channel);
    deinitSerialMsg(&smsg);


    // struct Message msg;
    // msg.type = M_Key;
    // msg.msg.key = K_A;
    // printMessage(&msg);
    // msg.type = M_ServerClose;
    // // msg.msg.key = K_A;
    // printMessage(&msg);
    // msg.type = M_String;
    // msg.msg.str = "this is a message string";
    // printMessage(&msg);
    

    printf("Finished\n");


}