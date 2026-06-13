

#include <stdio.h>
#include <keycode.h>

const char *whiteSpaceKeys[] = {
    // white space keys
    "Tab",
    "Space",
    "Return",
    "Delete",
    "BackSpace",
    "Shift",
    "LShift",
    "RShift",
    "Capital",
};

const char *controlKeys[] = {
    // Control keys
    "Escape",
    "Control",
    "LControl",
    "RControl",
    "Menu",
    "LWindows",
    "RWindows",
    "Alt",
    "LAlt",
    "RAlt",
};


const char *editingKeys[] = {
    // Editing keys
    "PageUp",
    "PageDown",
    "End",
    "Home",
    "Select",
    "Insert",
};


const char *cursorMovementKeys[] = {
    // Cursor movement keys
    "Left",
    "Up",
    "Right",
    "Down",
};

const char *alphanumericKeys[] = {
    // Alphanumeric keys
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "G",
    "H",
    "I",
    "J",
    "K",
    "L",
    "M",
    "N",
    "O",
    "P",
    "Q",
    "R",
    "S",
    "T",
    "U",
    "V",
    "W",
    "X",
    "Y",
    "Z",
};

const char *specialCharacterKeys[] = {
    // Special character keys
    // (note the naming convention is based on US keyboard layout,
    //  K_<non shifted char>_<shifted char>)
    "Semicolon_Colon",
    "Plus_Equal",
    "Comma_LAngleBracket",
    "Minus_Underscore",
    "Period_RAngleBracket",
    "ForwardSlash_QuestionMark",
    "Grave_Tilde",
    "LSquareBracket_LCurlyBracket",
    "Backslash_Pipe",
    "RSquareBracket_RCurlyBracket",
    "SingleQuote_DoubleQuote",
};

const char *mouseKeys[] = {
    // Mouse keys
    "Scroll",
    "ScrollUp",
    "ScrollDown",
    "LeftMouse",
    "RightMouse",
};

const char *numpadKeys[] = {
    // Numpad keys
    "Numpad0",
    "Numpad1",
    "Numpad2",
    "Numpad3",
    "Numpad4",
    "Numpad5",
    "Numpad6",
    "Numpad7",
    "Numpad8",
    "Numpad9",
    "Multiply",
    "Add",
    "Subtract",
    "Decimal",
    "Divide",
    "Equal",
    "Numlock",
};

const char *functionKeys[] = {
    // Function keys
    "F1",
    "F1",
    "F2",
    "F2",
    "F3",
    "F3",
    "F4",
    "F4",
    "F5",
    "F5",
    "F6",
    "F6",
    "F7",
    "F7",
    "F8",
    "F8",
    "F9",
    "F9",
    "F10",
    "F10",
    "F11",
    "F11",
    "F12",
    "F13",
    "F14",
    "F15",
    "F16",
    "F17",
    "F18",
    "F19",
    "F20",
    "F21",
    "F22",
    "F23",
    "F24",
};

const char *mediaKeys[] = {
    // Media and audio keys
    // Note currentl linux keys are unconfirmed
    "VolumeMute",
    "VolumeDown",
    "VolumeUp",
    "MediaNext",
    "MediaPrevTrack",
    "MediaStop",
    "MediaPlayPause",
};

const char *mscKeys[] = {
    // Msc keys
    "Print",
    "Execute",
    "Snapshot",
    "Help",
    "Apps",
    "Sleep",
};

void printSection(const char *name, const char **keycodes, unsigned int size) {
    printf("\n%s:\n", name);
    for (int i = 0; i < size/sizeof(const char *); i++) {
        printf("key '%s'\n", keycodes[i]);
    }
}

void printKeys() {
    printSection("White space keys", whiteSpaceKeys, sizeof(whiteSpaceKeys));

    printSection("Control keys", controlKeys, sizeof(controlKeys));

    printSection("Editing keys", editingKeys, sizeof(editingKeys));

    printSection("Cursor movement keys", cursorMovementKeys, sizeof(cursorMovementKeys));

    printSection("Alphanumeric keys", alphanumericKeys, sizeof(alphanumericKeys));

    printSection("Special character keys", specialCharacterKeys, sizeof(specialCharacterKeys));

    printSection("Mouse keys", mouseKeys, sizeof(mouseKeys));

    printSection("Numpad keys", numpadKeys, sizeof(numpadKeys));

    printSection("Function keys", functionKeys, sizeof(functionKeys));

    printSection("Media keys", mediaKeys, sizeof(mediaKeys));

    printSection("Msc keys", mscKeys, sizeof(mscKeys));
}