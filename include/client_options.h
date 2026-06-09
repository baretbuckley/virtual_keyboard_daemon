#ifndef CLIENTOPTIONS_H
#define CLIENTOPTIONS_H

#include "keycode.h"

#include <stddef.h>


enum CommandType {
    CMD_REPEAT = 1,
    CMD_CLOSE_SERVER = 2,
    CMD_TYPE = 3,
    CMD_PRESS = 4,
    CMD_DELAY = 5,
    CMD_RELEASE = 6,
    FLAG_HELP = -2, // help flag is returned in place of comand when encountered
    CMD_UNKNOWN = 0,
};



struct RepeatCmdContext {
    int count;
    int repeat_delay_ms;
};
static struct RepeatCmdContext default_repeat_context = {0, 0};


struct TypeCmdContext {
    const char* message;
    int char_delay_ms;
};
static struct TypeCmdContext default_type_context = {"", 0};


struct PressCmdContext {
    union {
        const char *key;
        unsigned int keycode;
    };
    unsigned int press_delay_ms;
    unsigned char hold;
    unsigned char byKeycode;
};
static struct PressCmdContext default_press_context = {"", 0, 0, 0};


struct DelayCmdContext {
    unsigned int delay_ms;
};
static struct DelayCmdContext DEFAULT_DELAY_CONTEXT = {0};


struct ReleaseCmdContext {
    const char *key;
};
static struct ReleaseCmdContext DEFAULT_RELEASE_CONTEXT = {""};

union CmdContext {
    struct TypeCmdContext type;
    struct PressCmdContext press;
    struct RepeatCmdContext repeat;
    struct DelayCmdContext delay;
    struct ReleaseCmdContext release;
};

enum CommandParseResult {
    CMD_PARSE_SUCCESS = 0,
    CMD_PARSE_ERROR = -1,
    CMD_PARSE_HELP = -2,
};

// enum CommandParseResult parse_repeat_command(const char** input, struct RepeatCmdContext* context, int *read);


// Parse subcommand options and argument, (i.e. press, type, repeat, delay, and release)
// Options must be in format <Command Name> [Options]... [arg]
enum CommandType parseCommand(const char** input, union CmdContext* context, int *read);



struct ClientOptContext {
    // channel name is extension of a base path dependant on OS
    const char *channelName;
    // channel path is a complete path used for channel location, only available on linux
    const char *channelPath;
};
static struct ClientOptContext DEFAULT_CLIENT_CONTEXT = {NULL, NULL};


// Parse client options
// --channel-name <string> -> context->channel-name
// --linux-channel-path <path> -> context->channel-path
int parseClientOptions(const char **input, struct ClientOptContext *context, int *read);



#endif // CLIENTOPTIONS_H

