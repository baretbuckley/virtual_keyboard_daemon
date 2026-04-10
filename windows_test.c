#include <wtypes.h> // Needed by winuser
#include <winuser.h>
#include <winerror.h>
#include <stdio.h>

#include "virtual_keyboard.h"
#include "keycode.h"

// void send_key() {
//     printf("Sending 'Win-D'\r\n");
//     INPUT inputs[4] = {};
//     ZeroMemory(inputs, sizeof(inputs));

//     inputs[0].type = INPUT_KEYBOARD;
//     inputs[0].ki.wVk = 'A';
   
//     inputs[1].type = INPUT_KEYBOARD;
//     inputs[1].ki.wVk = 'A';
//     inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;


//     inputs[2].type = INPUT_KEYBOARD;
//     inputs[2].ki.wVk = VK_OEM_PLUS;

//     inputs[3].type = INPUT_KEYBOARD;
//     inputs[3].ki.wVk = VK_OEM_PLUS;
//     inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;


//     UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
//     if (uSent != ARRAYSIZE(inputs))
//     {
//         printf("SendInput failed: 0x%x\n", HRESULT_FROM_WIN32(GetLastError()));
//     }

// }

int main(int argc, char** argv) {
    KeyBoard *kb = createKeyBoard();
    if (argc == 2)
        typeString(kb, argv[1]);
    
    // for (int i = 0; i < 100; i++) {
    //     tapKey(kb, K_H);
    // }

}