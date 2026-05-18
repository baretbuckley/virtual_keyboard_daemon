#include <commands.h>

#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <parse_option.h>

// Comand usages ------------------------------------------

const char *FULL_USAGE =
"\nUsage: vkey [-c|--channel-name <name>] [Subcommands]...\n"
"Subcommands:\n";

const char* REPEAT_USAGE =
"\nrepeat                     Repeat the following command a specified number of times.\n"
"  -h, --help                   Show the help message for 'repeat' and exit\n"
"  -d, --repeat-delay <MS>      Delay in milliseconds between each repetition\n"
"  <count>                      Number of times to repeat the next command\n";

const char *CLOSE_SERVER_USAGE = 
"\nclose_server               Send message requesting the target server to close.\n"
"  (no further options)\n";

const char *TYPE_USAGE = 
"\ntype                        Repeat the following command a specified number of times.\n"
"  -h, --help                   Show the help message for the 'type' command and exit\n"
"  -d, --key-delay <ms>         Number of times to repeat the message\n"
"  <string>                     String to type\n";

const char *PRESS_USAGE =
"\npress                      Repeat the following command a specified number of times.\n"
"  --help                       Show help message for the 'press' subcommand\n"
"  -h, --hold                   Holds the key without releasing (use the release command to release the key)\n"
"  --hold-for <ms>              delay between pressing and releasing the key (not compatable with using --hold)\n"
"  <key>                        the key to press\n";

const char *DELAY_USAGE =
"\ndelay                      Delay the execution of all following commands.\n"
"  -h, --help                   Show help message for the 'delay' subcommand\n"
"  <ms>                         Time in ms to delay execution\n";

const char *RELEASE_USAGE =
"\nrelease                    Release a specified key after it has been held\n"
"  -h, --help                   Show the help message for the 'release' subcommand.\n"
"  <key>                        The key to release.\n";



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



void printSubCommandUsage(const char *usage) {
    printf(usage);
    printf("Run 'vkey --help' for full help message");
}


void printFullUsage() {
    printf(FULL_USAGE);
    printf(TYPE_USAGE);
    printf(PRESS_USAGE);
    printf(RELEASE_USAGE);
    printf(CLOSE_SERVER_USAGE);
    printf(REPEAT_USAGE);
    printf(DELAY_USAGE);
}

void reportRepeatedOption(const char **argv) {
    fprintf(stderr, "%s: Option '%s' is repeated\n", argv[0], argv[get_optind()]);
    fprintf(stderr, "Try 'vkey --help' for more informantion\n");
}

void reportInvalidPositiveIntArg(const char **argv, const char *argStr) {
    fprintf(stderr, "%s: Option '%s' required postive intiger argument, recieved '%s'\n", argv[0], argv[get_optind()], argStr);
    fprintf(stderr, "Try 'vkey --help' for more informantion\n");
}


#define NUM_OPTIONS(options) sizeof( options ) / sizeof(struct Option)


// Using the given set flag it checks if
// The flag has already been set, if so
// reports the error and returns parse error
// Otherwise it sets the flag in the SET_MEMBERS variable
// SET_FLAG     : power of 2 const (i.e. 1<<n)
// SET_MEMBERS  : unsigned intiger value for storing which flags have occured
#define ASSERT_NON_REPEAT(SET_FLAG, SET_MEMBERS) \
if ( SET_MEMBERS & SET_FLAG ) {     \
    reportRepeatedOption(input);    \
    return CMD_PARSE_ERROR;         \
}                                   \
SET_MEMBERS |= SET_FLAG;            


// Convert the the string ARG into an unsigned in and store in VALUE_STORE
// If ARG is illformated it reports the error and returns CMD_PARSE_ERROR
// At its core its a wrapper
// ARG          : String to read unsigned int from
// VALUE_STORE  : Variable to store resut in
#define READ_UNSIGNED_INT_ARG(ARG, VALUE_STORE) \
int res = parse_positive_int(ARG);                      \
if (res < 0) {                                          \
    reportInvalidPositiveIntArg(input, arg);            \
    return CMD_PARSE_ERROR;                             \
}                                                       \
VALUE_STORE = res;




static const struct Option REPEAT_OPTIONS[] = {
    {"repeat-delay", sizeof("repeat-delay")-1, 'd', 1}
};
int parse_repeat_command(const char** input, struct RepeatCmdContext* context, int *read) {
    unsigned int set_members = 0;
    enum MemberFlags {
        REPEAT_DELAY_SET = 1 << 0,
    };
    set_optind(1);
    *context = default_repeat_context;
    const char *arg;
    while (1) {
        int res = parse_opt(input, REPEAT_OPTIONS, NUM_OPTIONS(REPEAT_OPTIONS), &arg);
        switch (res) {
            case 0: // repeat-delay
                ASSERT_NON_REPEAT(REPEAT_DELAY_SET, set_members)
                READ_UNSIGNED_INT_ARG(arg, context->repeat_delay_ms)
                break;
                

            case OPT_PARSE_NOT_AN_OPT:
            case OPT_PARSE_END_OF_ARGS:
                goto break_opt_loop;

            case OPT_PARSE_ERROR:
                return -1;

            case OPT_PARSE_HELP:
                printSubCommandUsage(PRESS_USAGE);
                return -1;
        }
    }
    break_opt_loop:

    if (parse_arg(input, &arg)){
        fprintf(stderr, "repeat: missing subcommand argument <count>\n");
        fprintf(stderr, "Try 'vkey --help' for more information.\n");
        return -1;
    }
    
    context->count = parse_positive_int(arg);
    if (context->count < 0) {
        reportInvalidPositiveIntArg(input, arg);
        return CMD_PARSE_ERROR;
    }


    *read = get_optind()+1; // Plus one for the argument
    return 0;
}

static const struct Option TYPE_OPTIONS[] = {
    {"key-delay", sizeof("key-delay")-1, 'd', 1}
};

int parse_type_command(const char** input, struct TypeCmdContext* context, int *read) {
    unsigned int set_members = 0;
    enum MemberFlags {
        KEY_DELAY_SET = 1 << 0,
    };
    set_optind(1);
    *context = default_type_context;
    const char *arg;
    while (1) {
        int res = parse_opt(input, TYPE_OPTIONS, NUM_OPTIONS(TYPE_OPTIONS), &arg);
        switch (res) {
            case 0: // key-delay
                ASSERT_NON_REPEAT(KEY_DELAY_SET, set_members)
                READ_UNSIGNED_INT_ARG(arg, context->char_delay_ms)
                break;

            case OPT_PARSE_NOT_AN_OPT:
            case OPT_PARSE_END_OF_ARGS:
                goto break_opt_loop;

            case OPT_PARSE_ERROR:
                return -1;

            case OPT_PARSE_HELP:
                printSubCommandUsage(TYPE_USAGE);
                return -1;
            default:
                fprintf(stderr, "Unrecognized return in parsing opt: %i\n", res);
        }
    }
    break_opt_loop:

    if (parse_arg(input, &arg)) {
        fprintf(stderr, "repeat: missing subcommand argument <string>\n");
        fprintf(stderr, "Try 'vkey --help' for more information.\n");
        return -1;
    }
    context->message = arg;

    *read = get_optind()+1; // Plus one for the argument
    return 0;
}





static const struct Option PRESS_OPTIONS[] = {
    {"hold", sizeof("hold")-1, 'h', 0},
    {"hold-for", sizeof("hold-for")-1, '\0', 1}
};
int parse_press_command(const char** input, struct PressCmdContext* context, int *read) {
    unsigned int set_members = 0;
    enum MemberFlags {
        HOLD_SET = 1 << 0,
        HOLD_FOR_SET = 1 << 1,
    };
    set_optind(1);
    *context = default_press_context;
    const char *arg;
    while (1) {
        int res = parse_opt(input, PRESS_OPTIONS, NUM_OPTIONS(PRESS_OPTIONS), &arg);
        switch (res) {
            case 0: // hold
                ASSERT_NON_REPEAT(HOLD_SET, set_members)
                context->hold = 1;
                break;

            case 1: // hold-for
                ASSERT_NON_REPEAT(HOLD_FOR_SET, set_members)
                READ_UNSIGNED_INT_ARG(arg, context->press_delay_ms)
                break;

            case OPT_PARSE_NOT_AN_OPT:
            case OPT_PARSE_END_OF_ARGS:
                goto break_opt_loop;

            case OPT_PARSE_ERROR:
                return -1;

            case OPT_PARSE_HELP:
                printSubCommandUsage(PRESS_USAGE);
                return -1;
        }
    }
    break_opt_loop:

    if (parse_arg(input, &arg)) {
        fprintf(stderr, "press: missing subcommand argument <keycode>\n");
        fprintf(stderr, "Try 'vkey --help' for more information.\n");
        return -1;
    }
    context->key = arg;

    *read = get_optind()+1; // Plus one for the argument
    return 0;
}




int parse_delay_command(const char** input, struct DelayCmdContext* context, int *read) {
    set_optind(1);
    const char *arg;
    *context = DEFAULT_DELAY_CONTEXT;
    // Just parsing to handle --help option
    while (1) {
        int res = parse_opt(input, NULL, 0, &arg);
        switch (res) {
            case OPT_PARSE_NOT_AN_OPT:
            case OPT_PARSE_END_OF_ARGS:
                goto break_opt_loop;

            case OPT_PARSE_ERROR:
                return -1;

            case OPT_PARSE_HELP:
                printSubCommandUsage(DELAY_USAGE);
                return -1;
            default:
                fprintf(stderr, "Unrecognized return in parsing opt: %i\n", res);
        }
    }
    break_opt_loop:


    if (parse_arg(input, &arg)) {
        fprintf(stderr, "delay: missing subcommand argument <delay>.\n");
        fprintf(stderr, "Try 'vkey --help' for more information.\n");
        return -1;
    }
    int res = parse_positive_int(arg);
    if (res < 0) {
        fprintf(stderr, "delay: subcommand argument <delay> must be positive integer value, found '%s'.\n", arg);
        fprintf(stderr, "Try 'vkey --help' for more information.\n");
        return -1;
    }
    context->delay_ms = res;


    *read = get_optind() + 1;
    return 0;
}


int parse_release_command(const char** input, struct ReleaseCmdContext* context, int *read) {
    set_optind(1);
    const char *arg;
    *context = DEFAULT_RELEASE_CONTEXT;
    // Just parsing to handle --help option
    while (1) {
        int res = parse_opt(input, NULL, 0, &arg);
        switch (res) {
            case OPT_PARSE_NOT_AN_OPT:
            case OPT_PARSE_END_OF_ARGS:
                goto break_opt_loop;

            case OPT_PARSE_ERROR:
                return -1;

            case OPT_PARSE_HELP:
                printSubCommandUsage(RELEASE_USAGE);
                return -1;
            default:
                fprintf(stderr, "Unrecognized return in parsing opt: %i\n", res);
        }
    }
    break_opt_loop:


    if (parse_arg(input, &arg)) {
        fprintf(stderr, "release: missing subcommand argument <key>.\n");
        fprintf(stderr, "Try 'vkey --help' for more information.\n");
        return -1;
    }
    context->key = arg;

    *read = get_optind() + 1;
    return 0;
}


enum CommandType parse_command(const char** input, union CmdContext* context, int *read) {
    printf("Checking sub command %s.\n", input[0]);
    if (strcmp((*input), "repeat") == 0) {
        if (parse_repeat_command(input, &(context->repeat), read))
            return CMD_UNKNOWN;
        return CMD_REPEAT;

    } else if (strcmp((*input), "close-server") == 0) {
        *read = 1;
        return CMD_CLOSE_SERVER;

    } else if (strcmp(input[0], "type") == 0) {
        if (parse_type_command(input, &(context->type), read))
            return CMD_UNKNOWN;
        return CMD_TYPE;

    } else if (strcmp(input[0], "press") == 0) {
        if (parse_press_command(input, &(context->press), read))
            return CMD_UNKNOWN;
        return CMD_PRESS;
    } else if (strcmp(input[0], "delay") == 0) {
        if (parse_delay_command(input, &(context->delay), read))
            return CMD_UNKNOWN;
        return CMD_DELAY;
    } else if (strcmp(input[0], "release") == 0) {
        if (parse_release_command(input, &(context->release), read))
            return CMD_UNKNOWN;
        return CMD_RELEASE;
    } else if (strcmp(input[0], "-h") == 0 || strcmp(input[0], "--help") == 0) {
        printFullUsage();
        return FLAG_HELP;
    } else {
        return CMD_UNKNOWN;
    }
}