#ifndef KEYCODE_H
#define KEYCODE_H

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>
#define SELECT_KEY(LINUX, WINDOWS) WINDOWS

#elif defined(__linux__)

#include <linux/input-event-codes.h>
#define SELECT_KEY(LINUX, WINDOWS) LINUX

#endif

// // #define USE_LINUX_KEYCODE
// #define USE_WINDOWS_KEYCODE


// #if defined USE_LINUX_KEYCODE
// // #include <linux/input-event-codes.h>
// // #define SELECT_KEY(LINUX, WINDOWS) LINUX
// #elif defined USE_WINDOWS_KEYCODE
// #include <windows.h>
// #endif

// TODO: Transcripe to use WinUser.h macros rather than copying the constants
enum KeyCode {
    K_UNKNOWN             = 0x0, // *DEFAULT* (flag used in auto file gen to signal default/unknown keycode)
    K_Reserved            = SELECT_KEY( 0,                          0xFF),

    // white space keys
    K_Tab                 = SELECT_KEY( KEY_TAB,                    VK_TAB ),
    K_Space               = SELECT_KEY( KEY_SPACE,                  VK_SPACE ),
    K_Return              = SELECT_KEY( KEY_ENTER,                  VK_RETURN ),
    K_Delete              = SELECT_KEY( KEY_DELETE,                 VK_DELETE ),
    K_BackSpace           = SELECT_KEY( KEY_BACKSPACE,              VK_BACK ),
    K_Shift               = SELECT_KEY( KEY_LEFTSHIFT,              VK_SHIFT ),
    K_LShift              = SELECT_KEY( KEY_LEFTSHIFT,              VK_LSHIFT ),
    K_RShift              = SELECT_KEY( KEY_RIGHTSHIFT,             VK_RSHIFT ),
    K_Capital             = SELECT_KEY( KEY_CAPSLOCK,               VK_CAPITAL ),
    
    // Control keys
    K_Escape              = SELECT_KEY( KEY_ESC,                    VK_ESCAPE ),
    K_Control             = SELECT_KEY( KEY_LEFTCTRL,               VK_CONTROL ),
    K_LControl            = SELECT_KEY( KEY_LEFTCTRL,               VK_LCONTROL ),
    K_RControl            = SELECT_KEY( KEY_RIGHTCTRL,              VK_RCONTROL ),
    K_Menu                = SELECT_KEY( KEY_MENU,                   VK_MENU ), // Todo check if this is right.
    K_LWindows            = SELECT_KEY( KEY_LEFTMETA,               VK_LWIN ),
    K_RWindows            = SELECT_KEY( KEY_RIGHTMETA,              VK_RWIN ),
    K_Alt                 = SELECT_KEY( KEY_LEFTALT,                VK_MENU ),
    K_LAlt                = SELECT_KEY( KEY_LEFTALT,                VK_LMENU ),
    K_RAlt                = SELECT_KEY( KEY_RIGHTALT,               VK_RMENU ),
    
    // Editing keys
    K_PageUp              = SELECT_KEY( KEY_PAGEUP,                 VK_PRIOR ),
    K_PageDown            = SELECT_KEY( KEY_PAGEDOWN,               VK_NEXT ),
    K_End                 = SELECT_KEY( KEY_END,                    VK_END ),
    K_Home                = SELECT_KEY( KEY_HOME,                   VK_HOME ),
    K_Select              = SELECT_KEY( KEY_SELECT,                 VK_SELECT ),
    K_Insert              = SELECT_KEY( KEY_INSERT,                 VK_INSERT ),

    // Cursor movement keys
    K_Left                = SELECT_KEY( KEY_LEFT,                   VK_LEFT ),
    K_Up                  = SELECT_KEY( KEY_UP,                     VK_UP ),
    K_Right               = SELECT_KEY( KEY_RIGHT,                  VK_RIGHT ),
    K_Down                = SELECT_KEY( KEY_DOWN,                   VK_DOWN ),

    // Alphanumeric keys
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

    // Special character keys
    // (note the naming convention is based on US keyboard layout,
    //  K_<non shifted char>_<shifted char>)
    K_Semicolon_Colon     = SELECT_KEY( KEY_SEMICOLON,              VK_OEM_1 ), // ';:' for US
    K_Plus_Equal          = SELECT_KEY( KEY_EQUAL,                  VK_OEM_PLUS ), // '+' any country
    K_Comma_LAngleBracket           = SELECT_KEY( KEY_COMMA,                VK_OEM_COMMA ),   // ',' any country
    K_Minus_Underscore              = SELECT_KEY( KEY_MINUS,                VK_OEM_MINUS ),   // '-' any country
    K_Period_RAngleBracket          = SELECT_KEY( KEY_DOT,                  VK_OEM_PERIOD ),   // '.' any country
    K_ForwardSlash_QuestionMark     = SELECT_KEY( KEY_SLASH,                VK_OEM_2 ),   // '/?' for US
    K_Grave_Tilde                   = SELECT_KEY( KEY_GRAVE,                VK_OEM_3 ),   // '`~' for US
    K_LSquareBracket_LCurlyBracket  = SELECT_KEY( KEY_LEFTBRACE,            VK_OEM_4 ),  //  '[{' for US
    K_Backslash_Pipe                = SELECT_KEY( KEY_BACKSLASH,            VK_OEM_5 ),  //  '\|' for US
    K_RSquareBracket_RCurlyBracket  = SELECT_KEY( KEY_RIGHTBRACE,           VK_OEM_6 ),  //  ']}' for US
    K_SingleQuote_DoubleQuote       = SELECT_KEY( KEY_APOSTROPHE,           VK_OEM_7 ),  //  ''"' for US




    // Mouse keys
    K_Scroll              = SELECT_KEY( BTN_MIDDLE,                 VK_SCROLL ), // TODO: IDK what the key is supposed to be doing
    K_ScrollUp            = SELECT_KEY( KEY_SCROLLUP,               VK_SCROLL ), // TODO: What is the difference between scroll up and down? is it the actual scroll wheel rotating?
    K_ScrollDown          = SELECT_KEY( KEY_SCROLLDOWN,             VK_SCROLL ),
    K_LeftMouse           = SELECT_KEY( BTN_LEFT,                   VK_LBUTTON ),
    K_RightMouse          = SELECT_KEY( BTN_RIGHT,                  VK_RBUTTON ),

    // Numpad keys
    K_Numpad0             = SELECT_KEY( KEY_NUMERIC_0,              VK_NUMPAD0 ),
    K_Numpad1             = SELECT_KEY( KEY_NUMERIC_1,              VK_NUMPAD1 ),
    K_Numpad2             = SELECT_KEY( KEY_NUMERIC_2,              VK_NUMPAD2 ),
    K_Numpad3             = SELECT_KEY( KEY_NUMERIC_3,              VK_NUMPAD3 ),
    K_Numpad4             = SELECT_KEY( KEY_NUMERIC_4,              VK_NUMPAD4 ),
    K_Numpad5             = SELECT_KEY( KEY_NUMERIC_5,              VK_NUMPAD5 ),
    K_Numpad6             = SELECT_KEY( KEY_NUMERIC_6,              VK_NUMPAD6 ),
    K_Numpad7             = SELECT_KEY( KEY_NUMERIC_7,              VK_NUMPAD7 ),
    K_Numpad8             = SELECT_KEY( KEY_NUMERIC_8,              VK_NUMPAD8 ),
    K_Numpad9             = SELECT_KEY( KEY_NUMERIC_9,              VK_NUMPAD9 ),
    K_Multiply            = SELECT_KEY( KEY_KPASTERISK,             VK_MULTIPLY ),
    K_Add                 = SELECT_KEY( KEY_KPPLUS,                 VK_ADD ),
    K_Subtract            = SELECT_KEY( KEY_KPMINUS,                VK_SUBTRACT ),
    K_Decimal             = SELECT_KEY( KEY_KPDOT,                  VK_DECIMAL ),
    K_Divide              = SELECT_KEY( KEY_KPSLASH,                VK_DIVIDE ),
    K_Equal               = SELECT_KEY( 0,                          VK_OEM_NEC_EQUAL ), // '=' key on numpad // TODO: what is numpad equal equivilant
    K_Numlock             = SELECT_KEY( KEY_NUMLOCK,                VK_NUMLOCK ),
    
    // Function keys
    K_F1                  = SELECT_KEY( KEY_F1,                     VK_F1 ),
    K_F2                  = SELECT_KEY( KEY_F2,                     VK_F2 ),
    K_F3                  = SELECT_KEY( KEY_F3,                     VK_F3 ),
    K_F4                  = SELECT_KEY( KEY_F4,                     VK_F4 ),
    K_F5                  = SELECT_KEY( KEY_F5,                     VK_F5 ),
    K_F6                  = SELECT_KEY( KEY_F6,                     VK_F6 ),
    K_F7                  = SELECT_KEY( KEY_F7,                     VK_F7 ),
    K_F8                  = SELECT_KEY( KEY_F8,                     VK_F8 ),
    K_F9                  = SELECT_KEY( KEY_F9,                     VK_F9 ),
    K_F10                 = SELECT_KEY( KEY_F10,                    VK_F10 ),
    K_F11                 = SELECT_KEY( KEY_F11,                    VK_F11 ),
    K_F12                 = SELECT_KEY( KEY_F12,                    VK_F12 ),
    K_F13                 = SELECT_KEY( KEY_F13,                    VK_F13 ),
    K_F14                 = SELECT_KEY( KEY_F14,                    VK_F14 ),
    K_F15                 = SELECT_KEY( KEY_F15,                    VK_F15 ),
    K_F16                 = SELECT_KEY( KEY_F16,                    VK_F16 ),
    K_F17                 = SELECT_KEY( KEY_F17,                    VK_F17 ),
    K_F18                 = SELECT_KEY( KEY_F18,                    VK_F18 ),
    K_F19                 = SELECT_KEY( KEY_F19,                    VK_F19 ),
    K_F20                 = SELECT_KEY( KEY_F20,                    VK_F20 ),
    K_F21                 = SELECT_KEY( KEY_F21,                    VK_F21 ),
    K_F22                 = SELECT_KEY( KEY_F22,                    VK_F22 ),
    K_F23                 = SELECT_KEY( KEY_F23,                    VK_F23 ),
    K_F24                 = SELECT_KEY( KEY_F24,                    VK_F24 ),
    
    
    // Media and audio keys
    // Note currentl linux keys are unconfirmed
    K_VolumeMute          = SELECT_KEY( KEY_VOLUME_MUTE,            VK_VOLUME_MUTE ),
    K_VolumeDown          = SELECT_KEY( KEY_VOLUME_DOWN,            VK_VOLUME_DOWN ),
    K_VolumeUp            = SELECT_KEY( KEY_VOLUME_UP,              VK_VOLUME_UP ),
    K_MediaNext           = SELECT_KEY( KEY_MEDIA_NEXT_TRACK,       VK_MEDIA_NEXT_TRACK ),
    K_MediaPrevTrack      = SELECT_KEY( KEY_MEDIA_PREV_TRACK,       VK_MEDIA_PREV_TRACK ),
    K_MediaStop           = SELECT_KEY( KEY_MEDIA_STOP,             VK_MEDIA_STOP ),
    K_MediaPlayPause      = SELECT_KEY( KEY_MEDIA_PLAY_PAUSE,       VK_MEDIA_PLAY_PAUSE ),

    // Browser keys
    K_BrowserBack        = SELECT_KEY( 0,                          VK_BROWSER_BACK ),
    K_BrowserForward     = SELECT_KEY( 0,                          VK_BROWSER_FORWARD ),
    K_BrowserRefresh     = SELECT_KEY( 0,                          VK_BROWSER_REFRESH ),
    K_BrowserStop        = SELECT_KEY( 0,                          VK_BROWSER_STOP ),
    K_BrowserSearch      = SELECT_KEY( 0,                          VK_BROWSER_SEARCH ),
    K_BrowserFavorites   = SELECT_KEY( 0,                          VK_BROWSER_FAVORITES ),
    K_BrowserHome        = SELECT_KEY( 0,                          VK_BROWSER_HOME ),

    // Msc keys
    K_Print               = SELECT_KEY( KEY_PRINT,                  VK_PRINT ),
    K_Execute             = SELECT_KEY( 0,                          VK_EXECUTE ), // TODO: find replacement and if not do we need this key
    K_Snapshot            = SELECT_KEY( KEY_SELECTIVE_SCREENSHOT,   VK_SNAPSHOT ), // TODO: Check if this is right conversion
    K_Help                = SELECT_KEY( KEY_HELP,                   VK_HELP ),
    K_Apps                = SELECT_KEY( KEY_APPSELECT,              VK_APPS ), // TODO: Confirm
    K_Sleep               = SELECT_KEY( KEY_SLEEP,                  VK_SLEEP ),


};




const char* keycodeAsString(enum KeyCode code);

enum KeyCode charToKeyCode(char c, int *shifted);

enum KeyCode keycodeFromString(const char *keyName);

#endif