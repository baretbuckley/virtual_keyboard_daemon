#ifndef KEYCODE_WINDOWS_H
#define KEYCODE_WINDOWS_H

#include <linux/input-event-codes.h>
#define USE_LINUX_KEYCODE


#if defined USE_LINUX_KEYCODE
#define SELECT_KEY(LINUX, WINDOWS) LINUX
#elif defined USE_WINDOWS_KEYCODE
#define SELECT_KEY(LINUX, WINDOWS) WINDOWS
#endif

// TODO: Transcripe to use WinUser.h macros rather than copying the constants
enum KeyCode {
    K_UNKNOWN             = 0x0, // *DEFAULT* (flag used in auto file gen to signal default/unknown keycode)
    
    K_Tab                 = SELECT_KEY( KEY_TAB,                    VK_TAB ),
    K_Space               = SELECT_KEY( KEY_SPACE,                  0 ),
    K_Return              = SELECT_KEY( KEY_ENTER,                  0 ), // TODO: KEY_ENTER doesn't work for some reason
    K_Delete              = SELECT_KEY( KEY_DELETE,                 0 ),
    K_BackSpace           = SELECT_KEY( KEY_BACKSPACE,              0 ),
    K_Shift               = SELECT_KEY( KEY_LEFTSHIFT,              0 ),
    K_LShift              = SELECT_KEY( KEY_LEFTSHIFT,              0 ),
    K_RShift              = SELECT_KEY( KEY_RIGHTSHIFT,             0 ),
    K_Capital             = SELECT_KEY( KEY_CAPSLOCK,               0 ),

    K_Escape              = SELECT_KEY( KEY_ESC,                    0 ),
    K_Control             = SELECT_KEY( KEY_LEFTCTRL,               0 ),
    K_LControl            = SELECT_KEY( KEY_LEFTCTRL,               0 ),
    K_RControl            = SELECT_KEY( KEY_RIGHTCTRL,              0 ),
    K_Menu                = SELECT_KEY( KEY_MENU,                   0 ), // Todo check if this is right. Does windows use menue for alt?
    K_LWindows            = SELECT_KEY( KEY_LEFTMETA,               0 ),
    K_RWindows            = SELECT_KEY( KEY_RIGHTMETA,              0 ),

    K_Alt                 = SELECT_KEY( KEY_LEFTALT,                0 ),
    K_LAlt                = SELECT_KEY( KEY_LEFTALT,                0 ),
    K_RAlt                = SELECT_KEY( KEY_RIGHTALT,               0 ),
    
    K_PageUp              = SELECT_KEY( KEY_PAGEUP,                 0 ),
    K_PageDown            = SELECT_KEY( KEY_PAGEDOWN,               0 ),
    K_End                 = SELECT_KEY( KEY_END,                    0 ),
    K_Home                = SELECT_KEY( KEY_HOME,                   0 ),
    K_Select              = SELECT_KEY( KEY_SELECT,                 0 ),
    K_Insert              = SELECT_KEY( KEY_INSERT,                 0 ),

    K_Print               = SELECT_KEY( KEY_PRINT,                  0 ),
    K_Execute             = SELECT_KEY( 0,                          0 ), // TODO: find replacement and if not do we need this key
    K_Snapshot            = SELECT_KEY( KEY_SELECTIVE_SCREENSHOT,   0 ), // TODO: Check if this is right conversion
    K_Help                = SELECT_KEY( KEY_HELP,                   0 ),

    K_Left                = SELECT_KEY( KEY_LEFT,                   0 ),
    K_Up                  = SELECT_KEY( KEY_UP,                     0 ),
    K_Right               = SELECT_KEY( KEY_RIGHT,                  0 ),
    K_Down                = SELECT_KEY( KEY_DOWN,                   0 ),


    K_0                   = SELECT_KEY( KEY_0,                      '0' ),
    K_1                   = SELECT_KEY( KEY_1,                      '1' ),
    K_2                   = SELECT_KEY( KEY_2,                      '2' ),
    K_3                   = SELECT_KEY( KEY_3,                      '3' ),
    K_4                   = SELECT_KEY( KEY_4,                      '4' ),
    K_5                   = SELECT_KEY( KEY_5,                      '5' ),
    K_6                   = SELECT_KEY( KEY_6,                      '6' ),
    K_7                   = SELECT_KEY( KEY_7,                      '7' ),
    K_8                   = SELECT_KEY( KEY_8,                      '8' ),
    K_9                   = SELECT_KEY( KEY_9,                      '9' ),
    K_A                   = SELECT_KEY( KEY_A,                      'A' ),
    K_B                   = SELECT_KEY( KEY_B,                      'B' ),
    K_C                   = SELECT_KEY( KEY_C,                      'C' ),
    K_D                   = SELECT_KEY( KEY_D,                      'D' ),
    K_E                   = SELECT_KEY( KEY_E,                      'E' ),
    K_F                   = SELECT_KEY( KEY_F,                      'F' ),
    K_G                   = SELECT_KEY( KEY_G,                      'G' ),
    K_H                   = SELECT_KEY( KEY_H,                      'H' ),
    K_I                   = SELECT_KEY( KEY_I,                      'I' ),
    K_J                   = SELECT_KEY( KEY_J,                      'J' ),
    K_K                   = SELECT_KEY( KEY_K,                      'K' ),
    K_L                   = SELECT_KEY( KEY_L,                      'L' ),
    K_M                   = SELECT_KEY( KEY_M,                      'M' ),
    K_N                   = SELECT_KEY( KEY_N,                      'N' ),
    K_O                   = SELECT_KEY( KEY_O,                      'O' ),
    K_P                   = SELECT_KEY( KEY_P,                      'P' ),
    K_Q                   = SELECT_KEY( KEY_Q,                      'Q' ),
    K_R                   = SELECT_KEY( KEY_R,                      'R' ),
    K_S                   = SELECT_KEY( KEY_S,                      'S' ),
    K_T                   = SELECT_KEY( KEY_T,                      'T' ),
    K_U                   = SELECT_KEY( KEY_U,                      'U' ),
    K_V                   = SELECT_KEY( KEY_V,                      'V' ),
    K_W                   = SELECT_KEY( KEY_W,                      'W' ),
    K_X                   = SELECT_KEY( KEY_X,                      'X' ),
    K_Y                   = SELECT_KEY( KEY_Y,                      'Y' ),
    K_Z                   = SELECT_KEY( KEY_Z,                      'Z' ),


    K_Apps                = SELECT_KEY( KEY_APPSELECT,              0 ), // TODO: Confirm
    K_Sleep               = SELECT_KEY( KEY_SLEEP,                  0 ),


    K_Scroll              = SELECT_KEY( BTN_MIDDLE,                 0 ), // TODO: IDK what the key is supposed to be doing
    K_ScrollUp            = SELECT_KEY( KEY_SCROLLUP,               0 ),
    K_ScrollDown          = SELECT_KEY( KEY_SCROLLDOWN,             0 ),
    K_LeftMouse           = SELECT_KEY( BTN_LEFT,                   0 ),
    K_RightMouse          = SELECT_KEY( BTN_RIGHT,                  0 ),


    K_Numpad0             = SELECT_KEY( KEY_NUMERIC_0,              0 ),
    K_Numpad1             = SELECT_KEY( KEY_NUMERIC_1,              0 ),
    K_Numpad2             = SELECT_KEY( KEY_NUMERIC_2,              0 ),
    K_Numpad3             = SELECT_KEY( KEY_NUMERIC_3,              0 ),
    K_Numpad4             = SELECT_KEY( KEY_NUMERIC_4,              0 ),
    K_Numpad5             = SELECT_KEY( KEY_NUMERIC_5,              0 ),
    K_Numpad6             = SELECT_KEY( KEY_NUMERIC_6,              0 ),
    K_Numpad7             = SELECT_KEY( KEY_NUMERIC_7,              0 ),
    K_Numpad8             = SELECT_KEY( KEY_NUMERIC_8,              0 ),
    K_Numpad9             = SELECT_KEY( KEY_NUMERIC_9,              0 ),
    K_Multiply            = SELECT_KEY( KEY_KPASTERISK,             0 ),
    K_Add                 = SELECT_KEY( KEY_KPPLUS,                 0 ),
    K_Subtract            = SELECT_KEY( KEY_KPMINUS,                0 ),
    K_Decimal             = SELECT_KEY( KEY_KPDOT,                  0 ),
    K_Divide              = SELECT_KEY( KEY_KPSLASH,                0 ),
    K_F1                  = SELECT_KEY( KEY_F1,                     0 ),
    K_F2                  = SELECT_KEY( KEY_F2,                     0 ),
    K_F3                  = SELECT_KEY( KEY_F3,                     0 ),
    K_F4                  = SELECT_KEY( KEY_F4,                     0 ),
    K_F5                  = SELECT_KEY( KEY_F5,                     0 ),
    K_F6                  = SELECT_KEY( KEY_F6,                     0 ),
    K_F7                  = SELECT_KEY( KEY_F7,                     0 ),
    K_F8                  = SELECT_KEY( KEY_F8,                     0 ),
    K_F9                  = SELECT_KEY( KEY_F9,                     0 ),
    K_F10                 = SELECT_KEY( KEY_F10,                    0 ),
    K_F11                 = SELECT_KEY( KEY_F11,                    0 ),
    K_F12                 = SELECT_KEY( KEY_F12,                    0 ),
    K_F13                 = SELECT_KEY( KEY_F13,                    0 ),
    K_F14                 = SELECT_KEY( KEY_F14,                    0 ),
    K_F15                 = SELECT_KEY( KEY_F15,                    0 ),
    K_F16                 = SELECT_KEY( KEY_F16,                    0 ),
    K_F17                 = SELECT_KEY( KEY_F17,                    0 ),
    K_F18                 = SELECT_KEY( KEY_F18,                    0 ),
    K_F19                 = SELECT_KEY( KEY_F19,                    0 ),
    K_F20                 = SELECT_KEY( KEY_F20,                    0 ),
    K_F21                 = SELECT_KEY( KEY_F21,                    0 ),
    K_F22                 = SELECT_KEY( KEY_F22,                    0 ),
    K_F23                 = SELECT_KEY( KEY_F23,                    0 ),
    K_F24                 = SELECT_KEY( KEY_F24,                    0 ),
    K_Numlock             = SELECT_KEY( KEY_NUMLOCK,                0 ),
    K_Equal               = SELECT_KEY( 0,                          0 ), // '=' key on numpad // TODO: what is numpad equal equivilant
    K_Semicolon_Colon     = SELECT_KEY( KEY_SEMICOLON,              0 ), // ';:' for US
    K_Plus_Equal          = SELECT_KEY( KEY_EQUAL,                  0 ), // '+' any country
    K_Comma_LAngleBracket           = SELECT_KEY( KEY_COMMA,                0 ),   // ',' any country
    K_Minus_Underscore              = SELECT_KEY( KEY_MINUS,                0 ),   // '-' any country
    K_Period_RAngleBracket          = SELECT_KEY( KEY_DOT,                  0 ),   // '.' any country
    K_ForwardSlash_QuestionMark     = SELECT_KEY( KEY_SLASH,                0 ),   // '/?' for US
    K_Grave_Tilde                   = SELECT_KEY( KEY_GRAVE,                0 ),   // '`~' for US
    K_LSquareBracket_LCurlyBracket  = SELECT_KEY( KEY_LEFTBRACE,            0 ),  //  '[{' for US
    K_Backslash_Pipe                = SELECT_KEY( KEY_BACKSLASH,            0 ),  //  '\|' for US
    K_RSquareBracket_RCurlyBracket  = SELECT_KEY( KEY_RIGHTBRACE,           0 ),  //  ']}' for US
    K_SingleQuote_DoubleQuote       = SELECT_KEY( KEY_APOSTROPHE,           0 ),  //  ''"' for US


};

#endif // KEYCODE_WINDOWS_H

