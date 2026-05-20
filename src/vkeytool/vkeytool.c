#include <fcntl.h>
#include <linux/input.h>

#include "print-keys.c"
#include "key_reader.c"

#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <dirent.h>











void displayUsage() {

}


void listKeys() {
    printKeys();
}


int main(int argc, const char **argv) {
    if (argc < 2) {
        displayUsage();
        return -1;
    }

    if (strcmp(argv[1], "list-keys") == 0) {
        listKeys();
    } else if (strcmp(argv[1], "key-reader") == 0) {
        keyReader();
    }
}