// #define VK_LBUTTON        0x01
// #define VK_RBUTTON        0x02
// #define VK_CANCEL         0x03
// #define VK_MBUTTON        0x04    /* NOT contiguous with L & RBUTTON */
// #define VK_XBUTTON1       0x05    /* NOT contiguous with L & RBUTTON */
// #define VK_XBUTTON2       0x06    /* NOT contiguous with L & RBUTTON */

#define BackSpace           0x08  
#define Tab            0x09

// #define VK_CLEAR          0x0C

#define Return         0x0D
#define Shift          0x10
#define Control        0x11
#define ControlLeft     162
#define ControlRight    163

// #define VK_MENU           0x12
// #define VK_PAUSE          0x13
// #define VK_CAPITAL        0x14
// #define VK_KANA           0x15
// #define VK_HANGEUL        0x15  /* old name - should be here for compatibility */
// #define VK_HANGUL         0x15
// #define VK_IME_ON         0x16
// #define VK_JUNJA          0x17
// #define VK_FINAL          0x18
// #define VK_HANJA          0x19
// #define VK_KANJI          0x19
// #define VK_IME_OFF        0x1A
#define Escape         0x1B

// #define VK_CONVERT        0x1C
// #define VK_NONCONVERT     0x1D
// #define VK_ACCEPT         0x1E
// #define VK_MODECHANGE     0x1F

#define Space          0x20

#define PageDown          0x21
#define PageUp           0x22
#define End            0x23
#define Home           0x24
#define Left           0x25
#define Up             0x26
#define Right          0x27
#define Down           0x28

#define Select         0x29
#define Print          0x2A
#define Execute        0x2B
#define Snapshot       0x2C
#define Insert         0x2D
#define Delete         0x2E
#define Help           0x2F
#define LeftWindows           0x5B
#define RightWindows           0x5C
#define Apps           0x5D
#define Sleep          0x5F
#define Numpad0        0x60
#define Numpad1        0x61
#define Numpad2        0x62
#define Numpad3        0x63
#define Numpad4        0x64
#define Numpad5        0x65
#define Numpad6        0x66
#define Numpad7        0x67
#define Numpad8        0x68
#define Numpad9        0x69
#define Multiply       0x6A
#define Add            0x6B
// #define VK_SEPARATOR      0x6C
#define Subtract       0x6D
#define Decimal        0x6E
#define Divide         0x6F
#define F1             0x70
#define F2             0x71
#define F3             0x72
#define F4             0x73
#define F5             0x74
#define F6             0x75
#define F7             0x76
#define F8             0x77
#define F9             0x78
#define F10            0x79
#define F11            0x7A
#define F12            0x7B
#define F13            0x7C
#define F14            0x7D
#define F15            0x7E
#define F16            0x7F
#define F17            0x80
#define F18            0x81
#define F19            0x82
#define F20            0x83
#define F21            0x84
#define F22            0x85
#define F23            0x86
#define F24            0x87
// #define VK_NAVIGATION_VIEW     0x88 // reserved
// #define VK_NAVIGATION_MENU     0x89 // reserved
// #define VK_NAVIGATION_UP       0x8A // reserved
// #define VK_NAVIGATION_DOWN     0x8B // reserved
// #define VK_NAVIGATION_LEFT     0x8C // reserved
// #define VK_NAVIGATION_RIGHT    0x8D // reserved
// #define VK_NAVIGATION_ACCEPT   0x8E // reserved
// #define VK_NAVIGATION_CANCEL   0x8F // reserved
#define Numlock        0x90
#define Scroll         0x91
#define Equal  0x92   // '=' key on numpad
// #define VK_OEM_FJ_JISHO   0x92   // 'Dictionary' key
// #define VK_OEM_FJ_MASSHOU 0x93   // 'Unregister word' key
// #define VK_OEM_FJ_TOUROKU 0x94   // 'Register word' key
// #define VK_OEM_FJ_LOYA    0x95   // 'Left OYAYUBI' key
// #define VK_OEM_FJ_ROYA    0x96   // 'Right OYAYUBI' key
// #define VK_BROWSER_BACK        0xA6
// #define VK_BROWSER_FORWARD     0xA7
// #define VK_BROWSER_REFRESH     0xA8
// #define VK_BROWSER_STOP        0xA9
// #define VK_BROWSER_SEARCH      0xAA
// #define VK_BROWSER_FAVORITES   0xAB
// #define VK_BROWSER_HOME        0xAC
// #define VK_VOLUME_MUTE         0xAD
// #define VK_VOLUME_DOWN         0xAE
// #define VK_VOLUME_UP           0xAF
// #define VK_MEDIA_NEXT_TRACK    0xB0
// #define VK_MEDIA_PREV_TRACK    0xB1
// #define VK_MEDIA_STOP          0xB2
// #define VK_MEDIA_PLAY_PAUSE    0xB3
// #define VK_LAUNCH_MAIL         0xB4
// #define VK_LAUNCH_MEDIA_SELECT 0xB5
// #define VK_LAUNCH_APP1         0xB6
// #define VK_LAUNCH_APP2         0xB7
#define Colin_Semicolon          0xBA   // ';:' for US
#define Plus_Equal        0xBB   // '+' any country
#define Comma_LAngleBracket  0xBC   // ',' any country
#define Minus_Underscore  0xBD   // '-' any country
#define Period_RAngleBracket     0xBE   // '.' any country
#define ForwardSlash_QuestionMark          0xBF   // '/?' for US
#define Grave_Tilde          0xC0   // '`~' for US
#define LSquareBracket_LCurlyBracket          0xDB  //  '[{' for US
#define Backslash_Pipe          0xDC  //  '\|' for US
#define RSquareBracket_RCurlyBracket          0xDD  //  ']}' for US
#define SingleQuote_DoubleQuote          0xDE  //  ''"' for US
// #define VK_OEM_8          0xDF
// #define VK_OEM_AX         0xE1  //  'AX' key on Japanese AX kbd
// #define VK_OEM_102        0xE2  //  "<>" or "\|" on RT 102-key kbd.
// #define VK_ICO_HELP       0xE3  //  Help key on ICO
// #define VK_ICO_00         0xE4  //  00 key on ICO
// #define VK_PROCESSKEY     0xE5
// #define VK_ICO_CLEAR      0xE6
// #define VK_PACKET         0xE7
// #define VK_OEM_RESET      0xE9
// #define VK_OEM_JUMP       0xEA
// #define VK_OEM_PA1        0xEB
// #define VK_OEM_PA2        0xEC
// #define VK_OEM_PA3        0xED
// #define VK_OEM_WSCTRL     0xEE
// #define VK_OEM_CUSEL      0xEF
// #define VK_OEM_ATTN       0xF0
// #define VK_OEM_FINISH     0xF1
// #define VK_OEM_COPY       0xF2
// #define VK_OEM_AUTO       0xF3
// #define VK_OEM_ENLW       0xF4
// #define VK_OEM_BACKTAB    0xF5
// #define VK_ATTN           0xF6
// #define VK_CRSEL          0xF7
// #define VK_EXSEL          0xF8
// #define VK_EREOF          0xF9
// #define VK_PLAY           0xFA
// #define VK_ZOOM           0xFB
// #define VK_NONAME         0xFC
// #define VK_PA1            0xFD
// #define VK_OEM_CLEAR      0xFE