#ifndef GETOPT_H
#define GETOPT_H






enum OptionParseReturn {
    OPT_PARSE_END_OF_ARGS = -1,
    OPT_PARSE_NOT_AN_OPT = -2,
    OPT_PARSE_ERROR = -3,
    OPT_PARSE_HELP = -4,
};

struct Option {
    const char * longName;
    unsigned int longNameLen;
    char shortName;
    unsigned char hasArg;
};

int parse_arg(const char **argv, const char **optarg);

// int parse_opt_long(const char **argv, const struct Option *options, unsigned int numOptions, const char **optarg);

// int parse_opt_short(const char **argv, const struct Option *options, unsigned int numOptions, const char **optarg);

// Parse an option from argv
// If the option supplies an argument, it is stored in optarg
// If the parse is successful, returns the index of the option in the options list
// On parse fail it returns a negative value from the enum OptionParseReturn
int parse_opt(const char **argv, const struct Option *options, unsigned int numOptions, const char **optarg);


int get_optind();

int set_optind(int new_optind);

#endif // GETOPT_H