#include "print_keys.c"

#if defined(_WIN32) || defined(_WIN64)
    #include "windows.c/key_reader.c"
#elif defined(__linux__)
    #include "linux.c/key_reader.c"
#else
    #error "Unsupported platform"
#endif

#include <string.h>

#include <stdio.h>
#include <stdlib.h>











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