#include "daemonize.h"

#include <unistd.h>
#include <systemd/sd-daemon.h>

int getFD() {
    int n = sd_listen_fds(0);
    if (n != 1) return -1;
    int listen_fd = SD_LISTEN_FDS_START;
    return listen_fd;
}