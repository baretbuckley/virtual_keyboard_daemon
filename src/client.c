#include <channel.h>
#include <message.h>
#include <client_options.h>

#include <stdio.h>
#include <string.h>
// #include <windows.h>

struct ClientChannel *channel;
unsigned char buffer[CHANNEL_BUFFER_SIZE]; // Message buffer used in the serial message


void cleanUp() {
    if (channel) {
        disconnect(channel);
        freeClientChannel(channel);
    }
}

void initChannel(const struct ClientOptContext *clientContext) {
    // open channel based on result of client options
    // channelPath (on linux) ->     <channelPath>
    // channelName ->                <DEFAULT PREFIX>/<channelName>
    // neither ->                    <DEFAULT PREFIX>/vkeyd

    // Only check if channelPath is provided if it is a linux system, otherwise just check channelName
#ifdef __linux__
    if (clientContext->channelPath != 0) {
        printf("Using channel path\n");
        channel = openChannelWithPath(clientContext->channelPath);
    }
    else
#endif
    if (clientContext->channelName != 0) {
        printf("Using channel name\n");
        channel = openChannel(clientContext->channelName);
    } else {
        printf("Using default name\n");
        channel = openChannel("vkeyd");
    }

}

int sendSerialMessage(struct ClientChannel *channel, unsigned char *buffer, struct SerialMessage smsg) {
    int written;
    // prefix the buffer with the message len as u32
    *((uint32_t*)(buffer)) = smsg.msgLen;

    if ((written = sendMessage(channel, buffer, smsg.msgLen + 4)) < 0) {
        printf("Failed to write to file\n");
        disconnect(channel);
        freeClientChannel(channel);
        return -1;
    }

    printf("Sent message:\n");
    printSerialMsg(smsg);
    return 0;
}

int appendMessage(struct Message msg, struct SerialMessage *smsg) {
    if (serialMsgAppend(smsg, msg)) {
        // If failed to append, send current message and try again.
        // Before trying again, if the message is Type or TypeDelay,
        // check if length is greater than capacity and split into multiple messages if so

        // If any messages in the current smsg buffer, send them
        if (serialMsgLen(*smsg) > 0) {
        
            // Send what is currently in the buffer and clear it
            if (sendSerialMessage(channel, buffer, *smsg)) {
                cleanUp();
                return -1;
            }
            clearRetainingCapacity(smsg);

        }

        // If the message that failed to append is type or type delayed,
        // Try splitting it into multiple messages.
        if (msg.type == M_Type || msg.type == M_TypeDelay) {
            // If the message is a type message, check if it's length is within capacity
            // If not split it. and send it in segments
            unsigned int msgLen = strlen(msg.msg.str);
            unsigned int idx = 0;
            unsigned int sendLen = smsg->capacity-8; // -8 leaves space to add msg type (and delay if TypeDelay) into smsg

            while ((msgLen-idx) > sendLen) {
                printf("%i, %i, %i, %i\n", smsg->msgLen, smsg->capacity, msgLen-idx, sendLen);
                int err;
                if (msg.type == M_Type)
                    err = serialMsgAppendTypeN(smsg, msg.msg.str+idx, sendLen);
                else
                    err = serialMsgAppendTypeDelayN(smsg, msg.msg.str+idx, msg.delay, sendLen);
                if (err) {
                    fprintf(stderr, "vkey: Failed to append split message, message may be too large for buffer capacity\n");
                    cleanUp();
                    return -1;
                }

                if (sendSerialMessage(channel, buffer, *smsg)) {
                    cleanUp();
                    return -1;
                }
                clearRetainingCapacity(smsg);
                idx += sendLen;
                msg.msg.str += sendLen;
            }
        }

        // Lastly the message capacity should be sufficient for the message,
        // Give one last attept to append the message.
        if (serialMsgAppend(smsg, msg)) {
            // If type messages are split, this should not fail,
            // if it does likely an unhandled message type is too large.
            fprintf(stderr, "vkey: Failed to append message even after sending, message may be too large for buffer capacity\n");
            cleanUp();
            return -1;
        }
    }
    return 0;
}

void translateTypeCommand(struct TypeCmdContext context, struct Message *msg) {
    if (context.message == NULL) {
        printf("Null message found\n");
        return;
    }
    printf("Found subcommand 'type', delay = %i, message = '%s'\n", context.char_delay_ms, context.message);
    if (context.char_delay_ms) {
        msg->type = M_TypeDelay;
        msg->msg.str = context.message;
        msg->delay = context.char_delay_ms;
    } else {
        msg->type = M_Type;
        msg->msg.str = context.message;
    } 
}

int appendPressCommand(struct PressCmdContext context, struct SerialMessage *smsg) {
    struct Message msg;
    enum KeyCode key;

    // If by keycode, it should already be parsed as number value,
    // Otherwise convert the key string name into the correct keycode
    if (context.byKeycode) {
        printf("Found subcommand 'press', hold? = %i, hold-for = %i, keycode = %i\n", context.hold, context.press_delay_ms, context.keycode);
        key = context.keycode;
    } else {
        printf("Found subcommand 'press', hold? = %i, hold-for = %i, key = '%s'\n", context.hold, context.press_delay_ms, context.key);
        key = keycodeFromString(context.key);
        if (key == K_UNKNOWN) {
            printf("Unknown key '%s'", context.key);
            return 0;
        }
    }
    printf("Have keycode now\n");
    // if (context.hold) {
    //     if (context.press_delay_ms) {
    //         fprintf(stderr, "The press command does not allow the use of the --hold and --hold-for flag at the same time\nrun 'vkey --help' for usage");
    //         cleanUp();
    //         return -1;
    //     }
    //     msg.type = M_Hold;
    //     msg.msg.key = key;
    // } else 
    
    // If any special keys specified in command, hold them before adding target key's msg
    if (context.specialKeys) {
        msg.type = M_Hold;
        msg.delay = 0;
        if (PRESS_CMD_GET_SHIFT(context.specialKeys)) {
            msg.msg.key = K_Shift;
            if (appendMessage(msg, smsg) == -1) return -1;
        }
        if (PRESS_CMD_GET_CTRL(context.specialKeys)) {
            msg.msg.key = K_Control;
            if (appendMessage(msg, smsg) == -1) return -1;
        }
        if (PRESS_CMD_GET_ALT(context.specialKeys)) {
            msg.msg.key = K_Alt;
            if (appendMessage(msg, smsg) == -1) return -1;
        }
    }

    if (context.press_delay_ms) {
        msg.type = M_PressFor;
        msg.msg.key = key;
        msg.delay = context.press_delay_ms;
    } else {
        msg.type = M_Press;
        msg.msg.key = key;
    }
    if (appendMessage(msg, smsg) == -1) return -1;

    // If any special keys specified in command, release them after adding target key's msg
    if (context.specialKeys) {
        msg.type = M_Release;
        msg.delay = 0;
        if (PRESS_CMD_GET_SHIFT(context.specialKeys)) {
            msg.msg.key = K_Shift;
            if (appendMessage(msg, smsg) == -1) return -1;
        }
        if (PRESS_CMD_GET_CTRL(context.specialKeys)) {
            msg.msg.key = K_Control;
            if (appendMessage(msg, smsg) == -1) return -1;
        }
        if (PRESS_CMD_GET_ALT(context.specialKeys)) {
            msg.msg.key = K_Alt;
            if (appendMessage(msg, smsg) == -1) return -1;
        }
    }

    return 0;
}


int main(int argc, char** argv) {

    


    int totalRead;
    struct Message msg;

    // Parse vkey options

    struct ClientOptContext clientContext;
    if (parseClientOptions((const char**)argv, &clientContext, &totalRead)) {
        // If failed to parse, just imediatly terminate
        // any errors are reported in the parseClientOptions function
        return -1;
    }
    if (clientContext.channelName && clientContext.channelPath) {
        fprintf(stderr, "%s: setting both the --channel-name and --linux-channel-path options is not supported\n", argv[0]);
        fprintf(stderr, "Try 'vkey --help' for more information.\n");
        return -1;
    }
#ifndef __linux__
    if (clientContext.channelPath) {
        fprintf(stderr, "%s: option --linux-channel-path is not available for operating systems other than linux\n", argv[0]);
    }
#endif

    // Initialize the channel based on the client context
    initChannel(&clientContext);
    if (channel == NULL) {
        printf("Failed to open channel\n");
        cleanUp();
        return -1;
    }

    // Read commands from argv and build up serial message
    struct SerialMessage smsg = initSerialMsgFrom(buffer+4, CHANNEL_BUFFER_SIZE-4); // First 4 bytes excluded to allow prefix to allow length prefix to be added at the beginning
    enum KeyCode key;
    // Parse sub commands
    while (totalRead != argc) {
        int read = 0;
        union CmdContext context;
        printf("Total read: %i\n", totalRead);

        // Build msg object from command line arguments
        enum CommandType cmdType = parseCommand((const char**)(argv + totalRead), &context, &read);
        totalRead += read;
        switch (cmdType) {
            case CMD_TYPE:
                translateTypeCommand(context.type, &msg);
                break;
                
            case CMD_PRESS:
                // Appended directly as press command may be translated into multiple messages
                if (appendPressCommand(context.press, &smsg)) {
                    cleanUp();
                    return -1;
                }
                continue;
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

            case CMD_HOLD:
                printf("Found subcommand 'hold'.\n");
                if (context.hold.byKeycode) {
                    printf("Found subcommand 'hold', keycode = %i\n", context.hold.keycode);
                    key = context.hold.keycode;
                } else {
                    printf("Found subcommand 'press', key = '%s'\n", context.hold.key);
                    key = keycodeFromString(context.hold.key);
                    if (key == K_UNKNOWN) {
                        printf("Unknown key '%s'", context.hold.key);
                        return 0;
                    }
                }
                if (key == K_UNKNOWN) {
                    printf("Unknown key '%s'", context.press.key);
                    cleanUp();
                    return -1;
                }
                msg.type=M_Hold;
                msg.msg.key = key;
                break;

            case CMD_RELEASE:
                printf("Found subcommand 'release'.\n");
                key = keycodeFromString(context.press.key);
                if (key == K_UNKNOWN) {
                    printf("Unknown key '%s'", context.press.key);
                    cleanUp();
                    return -1;
                }
                msg.type=M_Release;
                msg.msg.key = key;
                break;
            
            case CMD_UNKNOWN:
                printf("%s: Found Unknown subcommand '%s'.\n", argv[0], argv[totalRead + read]);
                cleanUp();
                return -1;
            default:
                printf("Real unknown found\n");
                cleanUp();
                return -1;
        }
        if (appendMessage(msg, &smsg)) {
            return -1;
        }
        // if (serialMsgAppend(&smsg, msg)) {
        //     // If failed to append, send current message and try again.
            
        //     // // init the channel if not already done. 
        //     // if (channel == NULL) {
        //     //     initChannel(&clientContext);
        //     //     if (channel == NULL) {
        //     //         printf("Failed to open channel\n");
        //     //         cleanUp(channel);
        //     //         return -1;
        //     //     }
        //     // }

        //     // If any messages in the current smsg buffer, send them
        //     if (serialMsgLen(smsg) > 0) {
            
        //         // Send what is currently in the buffer and clear it
        //         if (sendSerialMessage(channel, buffer, smsg)) {
        //             cleanUp();
        //             return -1;
        //         }
        //         clearRetainingCapacity(&smsg);

        //     }

        //     // If the message that failed to append is type or type delayed,
        //     // Try splitting it into multiple messages.
        //     if (msg.type == M_Type || msg.type == M_TypeDelay) {
        //         // If the message is a type message, check if it's length is within capacity
        //         // If not split it. and send it in segments
        //         unsigned int msgLen = strlen(msg.msg.str);
        //         unsigned int idx = 0;
        //         unsigned int sendLen = smsg.capacity-16;

        //         while ((msgLen-idx) > sendLen) {
        //             printf("%i, %i, %i, %i\n", smsg.msgLen, smsg.capacity, msgLen-idx, sendLen);
        //             int err;
        //             if (msg.type == M_Type)
        //                 err = serialMsgAppendTypeN(&smsg, msg.msg.str+idx, sendLen);
        //             else
        //                 err = serialMsgAppendTypeDelayN(&smsg, msg.msg.str+idx, msg.delay, sendLen);
        //             if (err) {
        //                 fprintf(stderr, "%s: Failed to append split message, message may be too large for buffer capacity\n", argv[0]);
        //                 cleanUp();
        //                 return -1;
        //             }

        //             if (sendSerialMessage(channel, buffer, smsg)) {
        //                 cleanUp();
        //                 return -1;
        //             }
        //             clearRetainingCapacity(&smsg);
        //             idx += sendLen;
        //             msg.msg.str += sendLen;
        //         }
        //     }

        //     // Lastly the message capacity should be sufficient for the message,
        //     // Give one last attept to append the message.
        //     if (serialMsgAppend(&smsg, msg)) {
        //         // If type messages are split, this should not fail,
        //         // if it does likely an unhandled message type is too large.
        //         fprintf(stderr, "%s: Failed to append message even after sending, message may be too large for buffer capacity\n", argv[0]);
        //         cleanUp();
        //         return -1;
        //     }



        // }
    }

//     // open channel based on result of client options
//     // channelPath (on linux) ->     <channelPath>
//     // channelName ->                <DEFAULT PREFIX>/<channelName>
//     // neither ->                   <DEFAULT PREFIX>/vkeyd
//     printf("channel path: %li.\n", clientContext.channelPath);
//     printf("channel name: %li.\n", clientContext.channelName);
//     // Only check if channelPath is provided if it is a linux system, otherwise just check channelName
// #ifdef __linux__
//     if (clientContext.channelPath != 0) {
//         printf("Using channel path\n");
//         channel = openChannelWithPath(clientContext.channelPath);
//     }
//     else
// #endif
//     if (clientContext.channelName != 0) {
//         printf("Using channel name\n");
//         channel = openChannel(clientContext.channelName);
//     } else {
//         printf("Using default name\n");
//         channel = openChannel("vkeyd");
//     }


    // init the channel if not already done. 
    // if (channel == NULL) {
    //     initChannel(&channel, &clientContext);
    //     if (channel == NULL) {
    //         printf("Failed to open channel\n");
    //         cleanUp(channel);
    //         return -1;
    //     }
    // }


    if (smsg.msgLen != 0) {
        if (sendSerialMessage(channel, buffer, smsg)) {
            cleanUp();
            return -1;
        }
        // if ((written = sendMessage(channel, buffer, smsg)) < 0) {
        //     printf("Failed to write to file\n");
        //     disconnect(channel);
        //     freeClientChannel(channel);
        //     return -1;
        // }
        disconnect(channel);

        printf("Sent message:\n");
        printSerialMsg(smsg);
    } else {
        printf("Msg len of 0, not sending\n");
    }

    cleanUp();

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