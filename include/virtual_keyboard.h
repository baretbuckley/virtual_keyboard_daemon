#ifndef VIRTUAL_KEYBOARD_H
#define VIRTUAL_KEYBOARD_H

#include "keycode.h"

struct KeyBoard;

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

struct KeyBoard *createKeyBoard();
void deleteKeyBoard(struct KeyBoard *keyboard);

enum Result pressKey(struct KeyBoard *keyboard, enum KeyCode key);

enum Result releaseKey(struct KeyBoard *keyboard, enum KeyCode key);

// void tapKey(KeyBoard *keyboard, enum KeyCode key, int delay_ms);
enum Result tapKey(struct KeyBoard *keyboard, enum KeyCode key);


enum Result typeString(struct KeyBoard *keyboard, const char* str, unsigned int delay);

#endif