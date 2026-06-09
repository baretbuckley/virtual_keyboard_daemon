#ifndef DAEMONOPTIONS_H
#define DAEMONOPTIONS_H

#include "keycode.h"

#include <stddef.h>




struct DaemonOptContext {
    // channel name is extension of a base path dependant on OS
    const char *channelName;
    // channel path is a complete path used for channel location, only available on linux
    const char *channelPath;
};
static struct DaemonOptContext DEFAULT_DAEMON_CONTEXT = {NULL, NULL};


// Parse daemon options
// --channel-name <string> -> context->channel-name
// --linux-channel-path <path> -> context->channel-path
int parseDaemonOptions(const char **input, struct DaemonOptContext *context, int *read);



#endif // DAEMONOPTIONS_H

