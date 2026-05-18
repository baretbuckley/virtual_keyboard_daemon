#include <channel.h>
#include <message.h>
#include <commands.h>

#include <stdio.h>
// #include <windows.h>

void cleanUp(struct ClientChannel *channel, struct SerialMessage smsg) {
    freeClientChannel(channel);
    deinitSerialMsg(&smsg);
}

int main(int argc, char** argv) {

    

    struct SerialMessage smsg = initSerialMsgCapacity(2048);

    int totalRead = 1;
    struct Message msg;
    enum KeyCode key;
    while (totalRead != argc) {
        int read;
        union CmdContext context;
        printf("Total read: %i\n", totalRead);

        // Build msg object from command line arguments
        switch (parse_command((const char**)(argv + totalRead), &context, &read)) {
            case CMD_TYPE:
                if (context.type.message == NULL) {
                    printf("Null message found\n");
                    break;
                }
                printf("Found subcommand 'type', delay = %i, message = '%s'\n", context.type.char_delay_ms, context.type.message);
                if (context.type.char_delay_ms) {
                    msg.type = M_TypeDelay;
                    msg.msg.str = context.type.message;
                    msg.delay = context.type.char_delay_ms;
                } else {
                    msg.type = M_Type;
                    msg.msg.str = context.type.message;
                } 

                break;
            case CMD_PRESS:
                printf("Found subcommand 'press', hold? = %i, hold-for = %i, key = '%s'\n", context.press.hold, context.press.press_delay_ms, context.press.key);
                key = keycodeFromString(context.press.key);
                if (key == K_UNKNOWN) {
                    printf("Unknown key '%s'", context.press.key);
                    break;
                }
                if (context.press.hold) {
                    if (context.press.press_delay_ms) {
                        fprintf(stderr, "The press command does not allow the use of the --hold and --hold-for flag at the same time\nrun 'vkey --help' for usage");
                        return -1;
                    }
                    msg.type = M_Hold;
                    msg.msg.key = key;
                    // fprintf(stderr, "The --hold flag is being deactivated until the release command is supported");
                    // return -1;
                } else if (context.press.press_delay_ms) {
                    msg.type = M_PressFor;
                    msg.msg.key = key;
                    msg.delay = context.press.press_delay_ms;
                } else {
                    msg.type = M_Press;
                    msg.msg.key = key;
                }

                break;
            case CMD_CLOSE_SERVER:
                printf("Found subcommand 'close-server'\n");
                msg.type = M_ServerClose;
                break;
            
            case CMD_DELAY:
                printf("Found subcommand 'delay'");
                msg.type=M_Delay;
                msg.delay = context.delay.delay_ms;
                break;
            
            case CMD_REPEAT:
                printf("Found subcommand 'repeat'");
                msg.type=M_RepeatNext;
                msg.msg.count = context.repeat.count;
                msg.delay = context.repeat.repeat_delay_ms;
                break;

            case CMD_RELEASE:
                printf("Found subcommand 'release'.\n");
                key = keycodeFromString(context.press.key);
                if (key == K_UNKNOWN) {
                    printf("Unknown key '%s'", context.press.key);
                    return -1;
                }
                msg.type=M_Release;
                msg.msg.key = key;
                break;
            
            case CMD_UNKNOWN:
                printf("Found Unknown subcommand\n");
                deinitSerialMsg(&smsg);
                return -1;
            default:
                printf("Real unknown found\n");
                deinitSerialMsg(&smsg);
                return -1;
        }
        serialMsgAppend(&smsg, msg);
        totalRead += read;
    }


    struct ClientChannel *channel = openChannel("vkeyd");
    if (channel == NULL) {
        printf("Failed to open channel\n");
        return -1;
    }

    int written;
    if (smsg.msgLen != 0) {
        if ((written = sendMessage(channel, smsg)) < 0) {
            printf("Failed to write to file\n");
            disconnect(channel);
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