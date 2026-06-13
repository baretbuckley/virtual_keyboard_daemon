// #include <wtypes.h> // Needed by winuser
// #include <winuser.h>
// #include <winerror.h>
#include <stdio.h>

#include "virtual_keyboard.h"
#include "keycode.h"


int keyReader() {
    short keyState1[256];
    short keyState2[256];
    short *currentState = keyState1;
    short *lastState = keyState2;

    for (int key = 8; key <= 255; key++) {
        lastState[key] = GetAsyncKeyState(key);
    }

    while (1) {
        for (int key = 8; key <= 255; key++) {
            currentState[key] = GetAsyncKeyState(key);
        }

        for (int i = 8; i < 256; i++) {
            if ((lastState[i]&1) == (currentState[i]&1)) continue;
            const char *keyStr = keycodeAsString(i);
            if (currentState[i]&1) {
                printf("Keycode %s pressed", keycodeAsString(i));
            } else {
                printf("Keycode %s released", keycodeAsString(i));
            }
            if (!strcmp(keyStr, "Unknown"))
                printf("(%i)", i);
            printf("\n");
        }


        // Swap the current and last state
        if (currentState == keyState1) {
            currentState = keyState2;
            lastState = keyState1;
        } else {
            currentState = keyState1;
            lastState = keyState2;
        }
    }
}
