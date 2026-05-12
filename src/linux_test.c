#include <virtual_keyboard.h>
#include <unistd.h>



int main(void) {
    struct KeyBoard *kbd = createKeyBoard();
    sleep(1);
    tapKey(kbd, K_3);
    pressKey(kbd, K_Shift);
    tapKey(kbd, K_D);
    releaseKey(kbd, K_Shift);
    tapKey(kbd, K_LeftWindows);
    tapKey(kbd, K_Multiply);
    sleep(1);

    deleteKeyBoard(kbd);
    
}