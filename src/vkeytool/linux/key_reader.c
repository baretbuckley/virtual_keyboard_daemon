#include <keycode.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <dirent.h>
#include <linux/input.h>
#include <fcntl.h>
#include <string.h>





// Return if given device supports EV_KEY events, otherwise 0
int isKeyEventEnabled(int fd) {
    unsigned long evbit[(EV_MAX) / (8 * sizeof(unsigned long))+1] = {0};
    if (ioctl(fd, EVIOCGBIT(0, sizeof(evbit)), &evbit) < 0) {
        // printf("Failed to get event bits, %s\n", strerror(errno));
        return 0;
    }

    // Return index EV_KEY
    return evbit[EV_KEY / (8 * sizeof(unsigned long))] & (1UL << (EV_KEY % (8 * sizeof(unsigned long))));
}


struct KeyBoard {
    char path[100];
    char name[100];
};

struct KeyBoard *keyboards;
int numKeyboards;
int maxKeyboards;

int findKeyEventDevices() {
    DIR *d;
    struct dirent *dp;

    d = opendir("/dev/input/by-id");
    if (d == NULL) {
        printf("Failed to open /dev/input directory\n");
        return -1;
    }
    int maxDevices = 10;
    keyboards = malloc(sizeof(struct KeyBoard) * maxDevices);
    numKeyboards = 0;

    char filename_qfd[100] ;
    while ((dp = readdir(d))) {
        // if (strstr(dp->d_name, "event")) {
        if (1) {
            
            snprintf(filename_qfd, sizeof(filename_qfd), "/dev/input/by-id/%s", dp->d_name);
            int fd = open(filename_qfd, O_RDONLY);
            if (fd < 0) {
                printf("Failed to open device path '%s'\n", filename_qfd);
                continue;
            }
            if (isKeyEventEnabled(fd)) {
                numKeyboards++;
                if (numKeyboards > maxDevices) {
                    maxDevices *= 2;
                    keyboards = realloc(keyboards, sizeof(struct KeyBoard) * maxDevices);
                }
                // printf("[%i] Device path: %s\n", numKeyboards, filename_qfd);
                strncpy(keyboards[numKeyboards-1].path, filename_qfd, sizeof(keyboards[numKeyboards-1].path) - 1);
                keyboards[numKeyboards-1].path[sizeof(keyboards[numKeyboards-1].path) - 1] = '\0';
            
                if (ioctl(fd, EVIOCGNAME(sizeof(keyboards[numKeyboards-1].name)), keyboards[numKeyboards-1].name) < 0) {
                    printf("Failed to get device name for path '%s', %s\n", filename_qfd, strerror(errno));
                    strncpy(keyboards[numKeyboards-1].name, "Unknown", sizeof(keyboards[numKeyboards-1].name) - 1);
                    keyboards[numKeyboards-1].name[sizeof(keyboards[numKeyboards-1].name) - 1] = '\0';
                }
                // printf("[%i] Device name: %s\n", numKeyboards, keyboards[numKeyboards-1].name);
            }
            close(fd);
        }
    }
    return 0;
}

void displayDvices() {
    for (int i = 0; i < numKeyboards; i++) {
        printf("[%i]: %s\n", i, keyboards[i].name);
    }
}

int requestDeviceFromUser() {
    if (numKeyboards == 0) {
        printf("No key event devices found\n");
        return -1;
    }
    if (numKeyboards == 1) {
        printf("Reading from device '%s'\n", keyboards[0].name);
        return 0;
    }

    displayDvices();
    printf("Multiple devices found, please select one.\n");
    printf("Note: some keyboards may have multiple event devices of the same name.\n");
    int selectedDevice = -1;
    while (selectedDevice < 0 || selectedDevice >= numKeyboards) {
        printf("Select by number (0-%i): ", numKeyboards-1);
        scanf("%i", &selectedDevice);
        if (selectedDevice < 0 || selectedDevice >= numKeyboards) {
            printf("\nInvalid selection, try again.\n");
        }
    }
    return selectedDevice;
}


void readKeyEvents(struct KeyBoard *keyboard) {
    int fd = open(keyboard->path, O_RDONLY);
    if (fd < 0) {
        printf("Failed to open device path '%s'\n", keyboard->path);
        return;
    }
    struct input_event ev;
    while (1) {
        ssize_t n = read(fd, &ev, sizeof(ev));
        if (n == (ssize_t)sizeof(ev)) {
            if (ev.type == EV_KEY) {
                const char *keycode = keycodeAsString(ev.code);
                if (strcmp(keycode, "Unknown") == 0) {
                    printf("Unknown keycode %i ", ev.code);
                } else {
                    printf("keycode '%s' ", keycodeAsString(ev.code));
                }
                switch (ev.value) {
                    case 0: // Release
                        printf("released\n");
                        break;
                    case 1: // Press
                        printf("pressed\n");
                        break;
                    case 2: // Repeat
                        printf("repeated\n");
                        break;
                    default:
                        printf("unrecognized action\n");
                        break;
                }
                // printf("Key event: code=%u, value=%d\n", ev.code, ev.value);
            }
        } else {
            printf("Failed to read event, %s\n", strerror(errno));
            break;
        }
    }
}

void keyReader() {

    findKeyEventDevices();
    for (int i = 0; i < numKeyboards; i++) {
        printf("Device %i: %s\n", i, keyboards[i].path);
    }
    if (numKeyboards == 0) {
        printf("No key event devices found\n");
    }

    int selectedDevice = requestDeviceFromUser();

    readKeyEvents(&keyboards[selectedDevice]);
    free(keyboards);
    return;

}