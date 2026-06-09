#include <daemon_options.h>

#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <parse_option.h>

// Comand usages ------------------------------------------

const char *USAGE =
"\nUsage: vkeyd [-c|--channel-name <name> | --linux-channel-path <path>]\n"
"Options:\n"
"   -c, --channel-name <name>     name of the channel, must be unique if multiple instances of the daemon are running"
"   --linux-channel-name <path>   full path to open the channel, only available for linux.\n";






void printSubCommandUsage(const char *usage) {
    puts(usage);
    printf("Run 'vkeyd --help' for full help message");
}


void printUsage() {
    puts(USAGE);
}

void reportRepeatedOption(const char **argv) {
    fprintf(stderr, "%s: Option '%s' is repeated\n", argv[0], argv[get_optind()]);
    fprintf(stderr, "Try 'vkeyd --help' for more informantion\n");
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
    return -1;                      \
}                                   \
SET_MEMBERS |= SET_FLAG;            




static const struct Option DAEMON_OPTIONS[] = {
    {"channel-name", sizeof("channel-name")-1, 'c', 1},
    {"linux-channel-path", sizeof("linux-channel-path")-1, '\0', 1},
};
int parseDaemonOptions(const char **input, struct DaemonOptContext *context, int *read) {
    unsigned int set_members = 0;
    enum MemberFlags {
        CHANNEL_NAME_SET = 1 << 0,
        LINUX_CHANNEL_NAME_SET = 1 << 1,
    };
    set_optind(1);
    const char *arg = NULL;
    *context = DEFAULT_DAEMON_CONTEXT;
    while (1) {
        int res = parse_opt(input, DAEMON_OPTIONS, NUM_OPTIONS(DAEMON_OPTIONS), &arg);
        switch (res) {
            case 0: // channel-name
                ASSERT_NON_REPEAT(CHANNEL_NAME_SET, set_members)
                context->channelName = arg;
                break;

            case 1: // linux-channel-path
                ASSERT_NON_REPEAT(LINUX_CHANNEL_NAME_SET, set_members)
                context->channelPath = arg;
                break;

            case OPT_PARSE_NOT_AN_OPT:
            case OPT_PARSE_END_OF_ARGS:
                goto break_opt_loop;

            case OPT_PARSE_ERROR:
                return -1;

            case OPT_PARSE_HELP:
                printUsage();
                return -1;
            default:
                fprintf(stderr, "Unrecognized return in parsing opt: %i\n", res);
        }
    }
    break_opt_loop:


    *read = get_optind();
    return 0;
}