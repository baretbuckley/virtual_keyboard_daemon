#ifndef VIRTUAL_KEYBOARD_H
#define VIRTUAL_KEYBOARD_H

#include "keycode.h"

#include <stdint.h>

// TODO the max keys currently isn't accurate, should be tailored to specific os
#define MAX_KEYS 256

struct KeyBoard {
#ifdef __linux__
    uint64_t pressKeys[(MAX_KEYS + 63) / 64];
    int fd;
#elif defined(_WIN32) || defined(_WIN64)
    
#endif
};

enum Result {
    Success = 0,
    Fail = -1,
};

enum Action {
    Press, // Press and hold key
    Release, // Release pressed key
    Tap, // Tap key and release
};

struct Event {
    enum KeyCode code;
    enum Action keyAction;
};

int initKeyBoard(struct KeyBoard *keyboard);
void deinitKeyBoard(struct KeyBoard *keyboard);

enum Result pressKey(struct KeyBoard *keyboard, enum KeyCode key);

enum Result releaseKey(struct KeyBoard *keyboard, enum KeyCode key);

// void tapKey(KeyBoard *keyboard, enum KeyCode key, int delay_ms);
enum Result tapKey(struct KeyBoard *keyboard, enum KeyCode key);


enum Result typeString(struct KeyBoard *keyboard, const char* str, unsigned int delay);

void releaseAllKeys(struct KeyBoard *keyboard);

#endif