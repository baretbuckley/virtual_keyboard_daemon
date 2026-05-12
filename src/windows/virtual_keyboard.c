#include <virtual_keyboard.h>


#include <stdio.h>
#include <wtypes.h> // Needed by winuser
#include <winuser.h>
#include <winerror.h>

#include "keycode.h"


struct KeyBoard {
    INPUT sendBuffer[100];
};


KeyBoard *createKeyBoard() {
    return malloc(sizeof(KeyBoard));
}
void deleteKeyBoard(KeyBoard *keyboard) {
    free(keyboard);
}

enum Result pressKey(KeyBoard *keyboard, enum KeyCode key) {
    INPUT *input = (keyboard->sendBuffer);
    ZeroMemory(input, sizeof(INPUT));
    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wVk = key;
    UINT uSent = SendInput(1, input, sizeof(INPUT));
    if (uSent != 1) {
        printf("SendInput failed: 0x%x\n", HRESULT_FROM_WIN32(GetLastError()));
    }
    return Success;
}

enum Result releaseKey(KeyBoard *keyboard, enum KeyCode key) {
    INPUT *input = (keyboard->sendBuffer);
    ZeroMemory(input, sizeof(INPUT));
    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wVk = key;
    input[0].ki.dwFlags = KEYEVENTF_KEYUP;
    UINT uSent = SendInput(1, input, sizeof(INPUT));
    if (uSent != 1) {
        printf("SendInput failed: 0x%x\n", HRESULT_FROM_WIN32(GetLastError()));
    }
    return Success;
}

// void tapKey(KeyBoard *keyboard, enum KeyCode key, int delay_ms);
enum Result tapKey(KeyBoard *keyboard, enum KeyCode key) {
    printf("Sending key %s\n", keycodeAsString(key));
    INPUT *inputs = (keyboard->sendBuffer);
    ZeroMemory(inputs, sizeof(INPUT)*2);
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = key;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = key;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    UINT uSent = SendInput(2, inputs, sizeof(INPUT));
    if (uSent != 2) {
        printf("SendInput failed: 0x%x\n", HRESULT_FROM_WIN32(GetLastError()));
    }
    return Success;
}


enum Result drainBuffer(KeyBoard *keyboard, int len) {
    UINT uSent = SendInput(len, keyboard->sendBuffer, sizeof(INPUT));
    if (uSent != len) {
        printf("SendInput failed: 0x%x\n", HRESULT_FROM_WIN32(GetLastError()));
    }
    ZeroMemory(keyboard->sendBuffer, sizeof(INPUT)*ARRAYSIZE(keyboard->sendBuffer));
    return Success;
}

int addInputPress(KeyBoard *keyboard, int pos, enum KeyCode key) {
    keyboard->sendBuffer[pos].type = INPUT_KEYBOARD;
    keyboard->sendBuffer[pos].ki.wVk = key;
    if (pos == ARRAYSIZE(keyboard->sendBuffer)-1) {
        drainBuffer(keyboard, pos+1);
        return 0;
    }
    return ++pos;
}

int addInputRelease(KeyBoard *keyboard, int pos, enum KeyCode key) {
    keyboard->sendBuffer[pos].type = INPUT_KEYBOARD;
    keyboard->sendBuffer[pos].ki.wVk = key;
    keyboard->sendBuffer[pos].ki.dwFlags = KEYEVENTF_KEYUP;
    if (pos == ARRAYSIZE(keyboard->sendBuffer)-1) {
        drainBuffer(keyboard, pos+1);
        return 0;
    }
    return ++pos;
}



enum Result typeString(KeyBoard *keyboard, const char* str) {
    int pos = 0; // current possition in buffer for filling
    ZeroMemory(keyboard->sendBuffer, sizeof(INPUT)*ARRAYSIZE(keyboard->sendBuffer));
    INPUT *inputBuf = keyboard->sendBuffer;
    int shift = 0;
    int requiredShift = 0;
    for (int i = 0; str[i]; i++) {

        enum KeyCode key = charToKeyCode(str[i], &requiredShift);
        if (requiredShift != shift) {
            if (requiredShift)
                pos = addInputPress(keyboard, pos, K_Shift);
            else
                pos = addInputRelease(keyboard, pos, K_Shift);
            shift = requiredShift;
        }

        pos = addInputPress(keyboard, pos, key);
        pos = addInputRelease(keyboard, pos, key);
    }
    if (shift)
        pos = addInputRelease(keyboard, pos, K_Shift);
    if (pos)
        drainBuffer(keyboard, pos);
    return Success;
}

