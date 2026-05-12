#ifndef KEYCODE_H
#define KEYCODE_H

#include "windows/keycode_windows.h"


const char* keycodeAsString(enum KeyCode code);

enum KeyCode charToKeyCode(char c, int *shifted);

enum KeyCode keycodeFromString(const char *keyName);

#endif