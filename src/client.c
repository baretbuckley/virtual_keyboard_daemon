#include <channel.h>
#include <message.h>
#include <commands.h>

#include <stdio.h>
#include <windows.h>

void cleanUp(struct ClientChannel *channel, struct SerialMessage smsg) {
    freeClientChannel(channel);
    deinitSerialMsg(&smsg);
}

int main(int argc, char** argv) {

    struct ClientChannel *channel = openChannel("vkeyd");
    if (channel == NULL) {
        printf("Failed to open channel\n");
        freeClientChannel(channel);
        return -1;
    }

    struct SerialMessage smsg = initSerialMsg();

    int totalRead = 1;
    while (totalRead != argc) {
        int read;
        union CmdContext context;
        switch (parse_command(argv + totalRead, &context, &read)) {
            case CMD_TYPE:
                if (context.type.message == NULL) {
                    printf("Null message found\n");
                    break;
                }
                printf("Found subcommand 'type', delay = %i, message = '%s'\n", context.type.char_delay_ms, context.type.message);
                serialMsgAppendString(&smsg, context.type.message);

                break;
            case CMD_PRESS:
                printf("Found subcommand 'press', hold? = %i, hold-for = %i, message = '%s'\n", context.press.hold, context.press.press_delay_ms, context.press.key);
                enum KeyCode key = keycodeFromString(context.press.key);
                if (key == K_UNKNOWN) {
                    printf("Unknown key '%s'", context.press.key);
                    break;
                }
                serialMsgAppendKey(&smsg, key);

                break;
            case CMD_UNKNOWN:
                printf("Found Unknown subcommand\n");
                break;;
            default:
                printf("Real unknown found\n");
                break;
        }
        totalRead += read;
    }

    int written;
    if (smsg.msgLen != 0) {
        if ((written = sendMessage(channel, smsg)) < 0) {
            printf("Failed to write to file\n");
            freeClientChannel(channel);
            return -1;
        }
        disconnect(channel);

        printf("Sent message:\n");
        printSerialMsg(smsg);
    } else {
        printf("Msg len of 0, not sending\n");
    }

    freeClientChannel(channel);
    deinitSerialMsg(&smsg);

    // struct SerialMessage smsg = initSerialMsg();
    // // serialMsgAppendKey(&smsg, K_A);
    // // serialMsgAppendKey(&smsg, K_C);
    // // serialMsgAppendString(&smsg, "This is a string to be sent");
    

    // struct ClientChannel *channel = openChannel("vkeyd");

    // if (channel == NULL) {
    //     printf("Failed to open channel\n");
    //     freeClientChannel(channel);
    //     return -1;
    // }

    // long int written = 0;
    // for (int i = 1; i < argc; i++) {
    //     if (!strcmp(argv[i], "close")) {
    //         serialMsgAppendServerClose(&smsg);
    //     } else if (!strcmp(argv[i], "type")) {
    //         ++i;
    //         serialMsgAppendString(&smsg, argv[i]);
    //     } else if (!strcmp(argv[i], "press")) {
    //         ++i;
    //         if ((argv[i][0] >= 'A' && argv[i][0] <= 'Z') || (argv[i][0] >= '0' && argv[i][0] <= '9') ) {
    //             serialMsgAppendKey(&smsg, argv[i][0]);
    //         } else {
    //             printf("Invaild key %s\n", argv[i]);
    //         }
    //     }
        
    // }
    

    // if ((written = sendMessage(channel, smsg)) < 0) {
    //     printf("Failed to write to file\n");
    //     freeClientChannel(channel);
    //     return -1;
    // }
    // disconnect(channel);

    // printf("Sent message:\n");
    // printSerialMsg(smsg);

    // freeClientChannel(channel);
    // deinitSerialMsg(&smsg);


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