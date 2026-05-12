#ifndef COMMANDS_H
#define COMMANDS_H

// TMP
// #include <string>

#include "keycode.h"


enum CommandType {
    CMD_REPEAT = 1,
    CMD_CLOSE_SERVER = 2,
    CMD_TYPE = 3,
    CMD_PRESS = 4,
    CMD_DELAY = 5,
    FLAG_HELP = -2, // help flag is returned in place of comand when encountered
    CMD_UNKNOWN = 0,
};



struct RepeatCmdContext {
    int count;
    int repeat_delay_ms;
};
static struct RepeatCmdContext default_repeat_context = {0, 0};


// struct CloseServerCmdContext {
// };
// static struct CloseServerCmdContext default_close_server_context = {};


struct TypeCmdContext {
    const char* message;
    int char_delay_ms;
};
static struct TypeCmdContext default_type_context = {"", 0};


struct PressCmdContext {
    const char *key;
    unsigned int press_delay_ms;
    unsigned char hold;
};
static struct PressCmdContext default_press_context = {"", 0, 0};



struct DelayCmdContext {
    unsigned int press_delay_ms;
};
static struct PressCmdContext DEFAULT_DELAY_CONTEXT = {0};

union CmdContext {
    struct TypeCmdContext type;
    struct PressCmdContext press;
};



enum CommandParseResult {
    CMD_PARSE_SUCCESS = 0,
    CMD_PARSE_UNKNOWN_FLAG = -1,
    CMD_PARSE_REPEATED_FLAG = -2,
    CMD_PARSE_INVALID_FLAG_VALUE = -3, 
    CMD_PARSE_MISSING_VALUE = -4,
    CMD_PARSE_INVALID_VALUE = -5,
    CMD_PARSE_HELP = -6,
};

enum CommandParseResult parse_repeat_command(const char** input, struct RepeatCmdContext* context, int *read);


enum CommandType parse_command(const char** input, union CmdContext* context, int *read);







#endif // COMMANDS_H

