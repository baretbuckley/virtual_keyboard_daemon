#ifndef KEYCODE_H
#define KEYCODE_H

#if defined(_WIN32) || defined(_WIN64)
#include "windows/keycode_windows.h"
#elif defined(__linux__)
#include "linux/keycode_linux.h"
#endif


const char* keycodeAsString(enum KeyCode code);

enum KeyCode charToKeyCode(char c, int *shifted);

enum KeyCode keycodeFromString(const char *keyName);

#endif