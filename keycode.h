#ifndef KEYCODE_H
#define KEYCODE_H

#include "keycode_windows.h"


const char* keycodeAsString(enum KeyCode code) {
    switch (code) {
        case K_BackSpace:          return "BackSpace";
        case K_Tab:                return "Tab";
        case K_Return:             return "Return";
        case K_Shift:              return "Shift";
        case K_LShift:             return "LShift";
        case K_RShift:             return "RShift";
        case K_Control:            return "Control";
        case K_ControlLeft:        return "ControlLeft";
        case K_ControlRight:       return "ControlRight";
        case K_Menu:               return "Menu";
        case K_LMenu:              return "LMenu";
        case K_RMenu:              return "RMenu";
        case K_Capital:            return "Captial";
        case K_Escape:             return "Escape";
        case K_Space:              return "Space";
        case K_PageUp:             return "PageUp";
        case K_PageDown:           return "PageDown";
        case K_End:                return "End";
        case K_Home:               return "Home";
        case K_Left:               return "Left";
        case K_Up:                 return "Up";
        case K_Right:              return "Right";
        case K_Down:               return "Down";
        case K_Select:             return "Select";
        case K_Print:              return "Print";
        case K_Execute:            return "Execute";
        case K_Snapshot:           return "Snapshot";
        case K_Insert:             return "Insert";
        case K_Delete:             return "Delete";
        case K_Help:               return "Help";
        case K_0:                  return "0";
        case K_1:                  return "1";
        case K_2:                  return "2";
        case K_3:                  return "3";
        case K_4:                  return "4";
        case K_5:                  return "5";
        case K_6:                  return "6";
        case K_7:                  return "7";
        case K_8:                  return "8";
        case K_9:                  return "9";
        case K_A:                  return "A";
        case K_B:                  return "B";
        case K_C:                  return "C";
        case K_D:                  return "D";
        case K_E:                  return "E";
        case K_F:                  return "F";
        case K_G:                  return "G";
        case K_H:                  return "H";
        case K_I:                  return "I";
        case K_J:                  return "J";
        case K_K:                  return "K";
        case K_L:                  return "L";
        case K_M:                  return "M";
        case K_N:                  return "N";
        case K_O:                  return "O";
        case K_P:                  return "P";
        case K_Q:                  return "Q";
        case K_R:                  return "R";
        case K_S:                  return "S";
        case K_T:                  return "T";
        case K_U:                  return "U";
        case K_V:                  return "V";
        case K_W:                  return "W";
        case K_X:                  return "X";
        case K_Y:                  return "Y";
        case K_Z:                  return "Z";
        case K_LeftWindows:        return "LeftWindows";
        case K_RightWindows:       return "RightWindows";
        case K_Apps:               return "Apps";
        case K_Sleep:              return "Sleep";
        case K_Numpad0:            return "Numpad0";
        case K_Numpad1:            return "Numpad1";
        case K_Numpad2:            return "Numpad2";
        case K_Numpad3:            return "Numpad3";
        case K_Numpad4:            return "Numpad4";
        case K_Numpad5:            return "Numpad5";
        case K_Numpad6:            return "Numpad6";
        case K_Numpad7:            return "Numpad7";
        case K_Numpad8:            return "Numpad8";
        case K_Numpad9:            return "Numpad9";
        case K_Multiply:           return "Multiply";
        case K_Add:                return "Add";
        case K_Subtract:           return "Subtract";
        case K_Decimal:            return "Decimal";
        case K_Divide:             return "Divide";
        case K_F1:                 return "F1";
        case K_F2:                 return "F2";
        case K_F3:                 return "F3";
        case K_F4:                 return "F4";
        case K_F5:                 return "F5";
        case K_F6:                 return "F6";
        case K_F7:                 return "F7";
        case K_F8:                 return "F8";
        case K_F9:                 return "F9";
        case K_F10:                return "F10";
        case K_F11:                return "F11";
        case K_F12:                return "F12";
        case K_F13:                return "F13";
        case K_F14:                return "F14";
        case K_F15:                return "F15";
        case K_F16:                return "F16";
        case K_F17:                return "F17";
        case K_F18:                return "F18";
        case K_F19:                return "F19";
        case K_F20:                return "F20";
        case K_F21:                return "F21";
        case K_F22:                return "F22";
        case K_F23:                return "F23";
        case K_F24:                return "F24";
        case K_Numlock:            return "Numlock";
        case K_Scroll:             return "Scroll";
        case K_Equal:              return "Equal";
        case K_Colon_Semicolon:    return "Colon_Semicolon";
        case K_Plus_Equal:         return "Plus_Equal";
        case K_Comma_LAngleBracket:            return "Comma_LAngleBracket";
        case K_Minus_Underscore:               return "Minus_Underscore";
        case K_Period_RAngleBracket:           return "Period_RAngleBracket";
        case K_ForwardSlash_QuestionMark:      return "ForwardSlash_QuestionMark";
        case K_Grave_Tilde:                    return "Grave_Tilde";
        case K_LSquareBracket_LCurlyBracket:   return "LSquareBracket_LCurlyBracket";
        case K_Backslash_Pipe:                 return "Backslash_Pipe";
        case K_RSquareBracket_RCurlyBracket:   return "RSquareBracket_RCurlyBracket";
        case K_SingleQuote_DoubleQuote:        return "SingleQuote_DoubleQuote";
        default: return "Unknown";
    }
}

enum KeyCode charToKeyCode(char c, int *shifted) {
    
    int shift = 0;
    enum KeyCode code; 
    if (c >= '0' && c <= '9') {
        code = c;
    } else if (c >= 'a' && c <= 'z') {
        code = (c-'a') + 'A';
    } else if (c >= 'A' && c <= 'Z') {
        code = c;
        shift = 1;
    } else {
        switch (c) {
            case ' ': code = K_Space; break;
            case '!': code = K_1; shift = 1; break; 
            case '"': code = K_SingleQuote_DoubleQuote; shift = 1; break; 
            case '#': code = K_3; shift = 1; break; 
            case '$': code = K_4; shift = 1; break; 
            case '%': code = K_5; shift = 1; break; 
            case '&': code = K_7; shift = 1; break; 
            case '\'': code = K_SingleQuote_DoubleQuote; break; 
            case '(': code = K_9; shift = 1; break; 
            case ')': code = K_0; shift = 1; break; 
            case '*': code = K_8; shift = 1; break; 
            case '+': code = K_Plus_Equal; shift = 1; break; 
            case ',': code = K_Comma_LAngleBracket; break; 
            case '-': code = K_Minus_Underscore; break; 
            case '.': code = K_Period_RAngleBracket; break; 
            case '/': code = K_ForwardSlash_QuestionMark; break; 
            case ':': code = K_Colon_Semicolon; shift = 1; break; 
            case ';': code = K_Colon_Semicolon; break; 
            case '<': code = K_Comma_LAngleBracket; shift = 1; break; 
            case '=': code = K_Plus_Equal; break; 
            case '>': code = K_Period_RAngleBracket; shift = 1; break; 
            case '?': code = K_ForwardSlash_QuestionMark; shift = 1; break; 
            case '@': code = K_2; shift = 1; break; 
            case '[': code = K_LSquareBracket_LCurlyBracket; break; 
            case '\\': code = K_Backslash_Pipe; break; 
            case ']': code = K_RSquareBracket_RCurlyBracket; break; 
            case '^': code = K_6; shift = 1; break; 
            case '_': code = K_Minus_Underscore; shift = 1; break; 
            case '`': code = K_Grave_Tilde; break; 
            case '{': code = K_LSquareBracket_LCurlyBracket; shift = 1; break;
            case '|': code = K_Backslash_Pipe; shift = 1; break;
            case '}': code = K_RSquareBracket_RCurlyBracket; shift = 1; break;
            case '~': code = K_Grave_Tilde; shift = 1; break;
            default: code = K_UNKNOWN; break;
        }
    }
    if (shifted)
        *shifted = shift;
    return code; 
}

#endif