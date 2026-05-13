#include <commands.h>

#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Comand usages ------------------------------------------

const char* REPEATE_USAGE =
"\nrepeat                     Repeat the following command a specified number of times.\n"
"  -h, --help                   Show the help message for repeat and exit\n"
"  -c, --count <count>          Number of times to repeat the message\n"
"  -r, --repeat-delay <MS>      Delay in milliseconds between each repetition\n"
"  <command>                    The command to repeat\n";

const char *CLOSE_SERVER_USAGE = 
"\nclose_server               Send message requesting the target server to close.\n"
"  -h, --help                   Show the help message for close_server and exit\n";

const char *TYPE_USAGE = 
"\type                        Repeat the following command a specified number of times.\n"
"  -h, --help                   Show the help message for the \"type\" command and exit\n"
"  -d, --key-delay <ms>         Number of times to repeat the message\n"
"  <string>                     String to type\n";

const char *PRESS_USAGE =
"\npress                      Repeat the following command a specified number of times.\n"
"  -h, --hold                   Holds the key without releasing (use the release command to release the key)\n"
"  --hold-for <ms>              delay between pressing and releasing the key (not compatable with using --hold)\n"
"  <key>                        the key to press\n";

const char *DELAY_USAGE =
"\ndelay                      Delay the execution of all following commands.\n"
"  <ms>                         Time in ms to delay execution\n";

int parse_positive_int(const char *valueStr) {
    // Read in integer value and ensure it is valid
    int res = 0;
    // Bounds check incase input is '0'
    // If its a different input 0 result means invalid str
    if (valueStr[0] != '0' || valueStr[1] != '\0') {
        res = atoi(valueStr);
        if (res <= 0) {
            return -1;
        }
    }
    return res;
}


enum CommandParseResult parse_repeat_command(const char** input, struct RepeatCmdContext* context, int *read) {
    unsigned int set_members = 0;
    enum MemberFlags {
        COUNT_SET = 1 << 0,
        REPEAT_DELAY_SET = 1 << 1,
    };

    unsigned int idx = 1;
    while (input[idx] != NULL && input[idx][0] == '-') {
        switch (input[idx][1]) {
            case 'c': // -c
                // Ensure value not already set and str terminates after "-c" 
                if (set_members & COUNT_SET) {
                    *read = idx;
                    return CMD_PARSE_REPEATED_FLAG; // -c specified more than once
                }
                if (input[idx][2] != '\0') {
                    *read = idx;
                    return CMD_PARSE_UNKNOWN_FLAG; // Missing argument for -c
                }
                set_members &= COUNT_SET;
                ++idx;
                // Read in count value
                context->count = parse_positive_int(input[idx]);
                if (context->count < 0) {
                    *read = idx;
                    return CMD_PARSE_INVALID_VALUE;
                }


                break;
            case 'r': // --repeat-delay
                // Ensure value not already set and str terminates after "-r" 
                if (set_members & REPEAT_DELAY_SET) {
                    *read = idx;
                    return CMD_PARSE_REPEATED_FLAG; // -c specified more than once
                }
                if (input[idx][2] != '\0') {
                    *read = idx;
                    return CMD_PARSE_UNKNOWN_FLAG; // if "-r" is not end of string, its not recognized
                }
                set_members &= COUNT_SET;
                ++idx;
                // Read in count value
                context->repeat_delay_ms = parse_positive_int(input[idx]);
                if (context->repeat_delay_ms < 0) {
                    *read = idx;
                    return CMD_PARSE_INVALID_VALUE;
                }
                break;
            case 'h': // -h or --help
                return CMD_PARSE_HELP;

            
            case '-': // Parse full name of flags
                if (!strcmp(&input[idx][2], "count")) {
                    // Ensure value not already set and str terminates after "-c" 
                    if (set_members & COUNT_SET) {
                        *read = idx;
                        return CMD_PARSE_REPEATED_FLAG; // -c specified more than once
                    }
                    set_members &= COUNT_SET;
                    ++idx;
                    // Read in count value
                    context->count = parse_positive_int(input[idx]);
                    if (context->count < 0) {
                        *read = idx;
                        return CMD_PARSE_INVALID_VALUE;
                    }
                } else if (!strcmp(&input[idx][2], "count")) {
                    // Ensure value not already set and str terminates after "-r" 
                    if (set_members & REPEAT_DELAY_SET) {
                        *read = idx;
                        return CMD_PARSE_REPEATED_FLAG; // -c specified more than once
                    }
                    set_members &= COUNT_SET;
                    ++idx;
                    // Read in count value
                    context->repeat_delay_ms = parse_positive_int(input[idx]);
                    if (context->repeat_delay_ms < 0) {
                        *read = idx;
                        return CMD_PARSE_INVALID_VALUE;
                    }
                } else if (!strcmp(&input[idx][2], "help")) {
                    return CMD_PARSE_HELP;  
                } else {
                    *read = idx;
                    return CMD_PARSE_UNKNOWN_FLAG;
                }
                break;
            default:
                return CMD_PARSE_UNKNOWN_FLAG; // Unknown option
        }
        ++idx;
    }
    
    *read = idx;
    return CMD_PARSE_SUCCESS;
}




enum CommandParseResult parse_type_command(const char** input, struct TypeCmdContext* context, int *read) {
    unsigned int set_members = 0;
    enum MemberFlags {
        DELAY_SET = 1 << 0,
    };
    *context = default_type_context;

    unsigned int idx = 1;
    while (input[idx] != NULL && input[idx][0] == '-') {
        switch (input[idx][1]) {
            case 'd':
                // Ensure value not already set and str terminates after "-c" 
                if (set_members & DELAY_SET) {
                    *read = idx;
                    return CMD_PARSE_REPEATED_FLAG; // -c specified more than once
                }
                if (input[idx][2] != '\0') {
                    *read = idx;
                    return CMD_PARSE_UNKNOWN_FLAG; // Missing argument for -c
                }
                set_members &= DELAY_SET;
                ++idx;
                // Read in count value
                context->char_delay_ms = parse_positive_int(input[idx]);
                if (context->char_delay_ms < 0) {
                    *read = idx;
                    return CMD_PARSE_INVALID_VALUE;
                }


                break;
            case 'h': // -h
                return CMD_PARSE_HELP;

            
            case '-': // Parse full name of flags
                if (!strcmp(&input[idx][2], "key-delay")) {
                    // Ensure value not already set and str terminates after "-c" 
                    if (set_members & DELAY_SET) {
                        *read = idx;
                        return CMD_PARSE_REPEATED_FLAG; // -c specified more than once
                    }
                    set_members &= DELAY_SET;
                    ++idx;
                    // Read in count value
                    context->char_delay_ms = parse_positive_int(input[idx]);
                    if (context->char_delay_ms < 0) {
                        *read = idx;
                        return CMD_PARSE_INVALID_VALUE;
                    }

                } else if (!strcmp(&input[idx][2], "help")) {
                    return CMD_PARSE_HELP;  
                } else {
                    *read = idx;
                    return CMD_PARSE_UNKNOWN_FLAG;
                }
                break;
            default:
                return CMD_PARSE_UNKNOWN_FLAG; // Unknown option
        }
        ++idx;
    }

    if (input[idx] == NULL) {
        *read = idx;
        return CMD_PARSE_MISSING_VALUE;
    }

    context->message = input[idx];
    *read = idx+1;
    
    return CMD_PARSE_SUCCESS;
}







enum CommandParseResult parse_press_command(const char** input, struct PressCmdContext* context, int *read) {
    unsigned int set_members = 0;
    enum MemberFlags {
        HOLD_SET = 1 << 0,
        HOLD_FOR_SET = 1 << 1,
    };
    *context = default_press_context;

    unsigned int idx = 1;
    while (input[idx] != NULL && input[idx][0] == '-') {
        switch (input[idx][1]) {
            case 'h':
                // Ensure value not already set and str terminates after "-c" 
                if (set_members & HOLD_SET) {
                    *read = idx;
                    return CMD_PARSE_REPEATED_FLAG; // -c specified more than once
                }
                if (input[idx][2] != '\0') {
                    *read = idx;
                    return CMD_PARSE_UNKNOWN_FLAG; // Missing argument for -c
                }
                set_members &= HOLD_SET;
                // Read in count value
                context->hold = 1;
                break;

            
            case '-': // Parse full name of flags
                if (!strcmp(&input[idx][2], "hold-for")) {
                    // Ensure value not already set and str terminates after "-c" 
                    if (set_members & HOLD_FOR_SET) {
                        *read = idx;
                        return CMD_PARSE_REPEATED_FLAG; // -c specified more than once
                    }
                    set_members &= HOLD_FOR_SET;
                    ++idx;
                    // Read in count value
                    int res = parse_positive_int(input[idx]);
                    if (res < 0) {
                        *read = idx;
                        return CMD_PARSE_INVALID_VALUE;
                    }
                    context->press_delay_ms = res;

                } else if (!strcmp(&input[idx][2], "hold")) {
                    // Ensure value not already set and str terminates after "-r" 
                    if (set_members & HOLD_SET) {
                        *read = idx;
                        return CMD_PARSE_REPEATED_FLAG; // -c specified more than once
                    }
                    set_members &= HOLD_SET;
                    context->hold = 1;
                } else if (!strcmp(&input[idx][2], "help")) {
                    return CMD_PARSE_HELP;  
                } else {
                    *read = idx;
                    return CMD_PARSE_UNKNOWN_FLAG;
                }
                break;
            default:
                return CMD_PARSE_UNKNOWN_FLAG; // Unknown option
        }
        ++idx;
    }

    // Read keycode
    if (!input[idx]) {
        *read = idx;
        return CMD_PARSE_MISSING_VALUE;
    }


    context->key = input[idx++];
    *read = idx;
    return CMD_PARSE_SUCCESS;
}


void printSubCommandUsage(const char *usage) {
    printf(usage);
    printf("Run 'vkey --help' for full help message");
}


void reportParseError(enum CommandParseResult result, const char ** input, int errorIdx) {
    switch (result) {
        case CMD_PARSE_UNKNOWN_FLAG:
            printf("Unrecognized flag '%s' in command %s.\n", input[errorIdx], input[0]);
            break;
        case CMD_PARSE_INVALID_VALUE:
            printf("Invalid flag value %s for flag %s.\n", input[errorIdx], input[errorIdx-1]);
            break;
        case CMD_PARSE_MISSING_VALUE:
            printf("Missing value for command %s.\n", input[0]);
            break;
        case CMD_PARSE_REPEATED_FLAG:
            printf("'%s' flag is repeated.", input[errorIdx]);
            break;
    }
    printf("Run vkey --help for command usage.\n");
}





enum CommandType parse_command(const char** input, union CmdContext* context, int *read) {
    // if (strcmp((*input), "repeat") == 0) {
    //     return CMD_REPEAT;
    // } else if (strcmp((*input), "close_server") == 0) {
    //     return CMD_CLOSE_SERVER;
    // } else 
    printf("Checking sub command %s.\n", input[0]);
    if (strcmp(input[0], "type") == 0) {
        enum CommandParseResult res = parse_type_command(input, &context->type, read);
        if (res == CMD_PARSE_SUCCESS)
            return  CMD_TYPE;
        else if (res == CMD_PARSE_HELP) {
            printSubCommandUsage(TYPE_USAGE);
        } else {
            reportParseError(res, input, *read);
        }
        return CMD_UNKNOWN;

    } else if (strcmp(input[0], "press") == 0) {
        enum CommandParseResult res = parse_press_command(input, &context->press, read);
        if (res == CMD_PARSE_SUCCESS)
            return  CMD_PRESS;
        else if (res == CMD_PARSE_HELP)
            printSubCommandUsage(PRESS_USAGE);
        else
            reportParseError(res, input, *read);
        return CMD_UNKNOWN;
    } else if (strcmp(input[0], "server-close") == 0) {
        *read = 1;
        return CMD_CLOSE_SERVER;

    

    } else if (strcmp(input[0], "-h") == 0 || strcmp(input[0], "--help") == 0) {
        return FLAG_HELP;
    } else {
        return CMD_UNKNOWN;
    }
}