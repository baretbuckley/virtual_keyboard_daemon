#include <parse_option.h>
#include <stdio.h>
#include <string.h>


int optind = 1;

enum OptionParseError {
    NOT_AN_OPTION = -1,           // Parsed argument wasn't an option (not prefixed by '-')
    END_OF_ARGS = -2,             // Reached end of args
    UNKNOWN_OPTION = -3,          // Encountered unspecified option
    UNEXPECTED_OPTION_ARG = -4,   // Option had an argument when not specified (i.e. through --option=<arg>)
    MISSING_ARG = -5,             // Option expected an arg but reached end of argv without one
    STOP_READING_OPTS = -6,       // Signal to stop reading options after this, mostly used to handle '--'
    HELP = -7,                    // Encountered help option while parsing
};

int parse_arg(const char **argv, const char **optarg) {
    if (argv[optind]) {
        *optarg = argv[optind];
        return 0;
    } else {
        return MISSING_ARG;
    }
}

int parse_opt_long(const char **argv, const struct Option *options, unsigned int numOptions, const char **optarg) {
    for (unsigned int i = 0; i < numOptions; i++) {
        printf("%s, %s\n", options[i].longName, (argv[optind]+2));
        if (!strncmp(options[i].longName, (argv[optind]+2), options[i].longNameLen)) {
            switch (argv[optind][options[i].longNameLen+2]) {
                case '\0':
                    printf("Found1 %s\n", &(argv[optind][options[i].longNameLen]));
                    if (!options[i].hasArg)
                        return i;
                    
                    optind += 1;
                    enum OptionParseError ret = parse_arg(argv, optarg);
                    if (ret == 0) {
                        return i;
                    } else {
                        return ret;
                    }
                case '=':
                    printf("Found2 %s\n", &(argv[optind][options[i].longNameLen]));
                    if (!options[i].hasArg)
                        return UNEXPECTED_OPTION_ARG;
                    
                    *optarg = &(argv[optind][options[i].longNameLen+3]); // arg is rest of string after '=' 
                    printf("arg is %s\n", *optarg);
                    return i;
                default:
                    printf("Found3 %s\n", &(argv[optind][options[i].longNameLen+2]));
                    break;
            }
        }
    }

    // If option is '--' read as end of options
    if (argv[optind][2] == '\0')
        return STOP_READING_OPTS;

    // Check if option is for help
    if (!strcmp((argv[optind]+2), "help"))
        return HELP;
    
    return UNKNOWN_OPTION;

}

int parse_opt_short(const char **argv, const struct Option *options, unsigned int numOptions, const char **optarg) {
    if (argv[optind][1] == 0) {
        return UNKNOWN_OPTION;
    }

    for (unsigned int i = 0; i < numOptions; i++) {
        if (options[i].shortName == argv[optind][1]) {
            if (argv[optind][2] != '\0')
                continue;
            
            if (!options[i].hasArg)
                return i;

            optind += 1;
            enum OptionParseError ret = parse_arg(argv, optarg);
            if (ret == 0)
                return i;
            else
                return ret; // if failed, forward the erro
            
        }
    }
    if (argv[optind][1] == 'h' && argv[optind][2] == '\0')
        return HELP;
    return UNKNOWN_OPTION;
}

// ase CMD_PARSE_UNKNOWN_FLAG:
//             printf("Unrecognized flag '%s' in command %s.\n", input[errorIdx], input[0]);
//             break;
//         case CMD_PARSE_INVALID_VALUE:
//             printf("Invalid flag value %s for flag %s.\n", input[errorIdx], input[errorIdx-1]);
//             break;
//         case CMD_PARSE_MISSING_VALUE:
//             printf("Missing value for command %s.\n", input[0]);
//             break;
//         case CMD_PARSE_REPEATED_FLAG:
//             printf("'%s' flag is repeated.", input[errorIdx]);
//             break;

enum OptionParseReturn processOptionError(const char **argv, enum OptionParseError error) {
    switch (error) {
        case NOT_AN_OPTION:
            return OPT_PARSE_NOT_AN_OPT;
        case END_OF_ARGS:
            return OPT_PARSE_END_OF_ARGS;

        case UNKNOWN_OPTION:
            fprintf(stderr, "%s: Unrecognized option '%s'.\n", argv[0], argv[optind]);
            break;
        case UNEXPECTED_OPTION_ARG:
            fprintf(stderr, "%s: Option '%s' an argument is supplied but is not supported by the flag.\n", argv[0], argv[optind]);
            break;
        case MISSING_ARG:
            fprintf(stderr, "%s: Missign argument for option '%s'.\n", argv[0], argv[optind-1]);
            break;
        case STOP_READING_OPTS:
            optind += 1;
            return OPT_PARSE_END_OF_ARGS;
        case HELP:
            return OPT_PARSE_HELP;
    }
    return OPT_PARSE_ERROR;
}

int parse_opt(const char **argv, const struct Option *options, unsigned int numOptions, const char **optarg) {

    if (argv[optind] == NULL)
        return OPT_PARSE_END_OF_ARGS;
    if (argv[optind][0] != '-') {
        return OPT_PARSE_NOT_AN_OPT;
    }
    int res;
    if (argv[optind][1] == '-') 
        res = parse_opt_long(argv, options, numOptions, optarg);
    else
        res = parse_opt_short(argv, options, numOptions, optarg);
    
    if (res >= 0) {
        optind += 1;
        return res;
    } else {
        return processOptionError(argv, res);
    }
}

int get_optind() {
    return optind;
}

int set_optind(int new_optind) {
    int last = optind;
    optind = new_optind;
    return last;
}