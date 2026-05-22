#include <message.h>
#include <stdint.h>
#include <stdio.h>
#include <keycode.h>
#include <stdlib.h>
#include <string.h>

void printMessage(struct Message *msg) {
    switch (msg->type) {
        case M_Type:
            printf("Type String: '%s'\n", msg->msg.str);
            break;
        case M_TypeDelay:
            printf("Type String: '%s' with a delay of %ims\n", msg->msg.str, msg->delay);
            break;
        case M_Press:
            printf("Tap key: %s\n", keycodeAsString(msg->msg.key));
            break;
        case M_Hold:
            printf("Hold key: %s\n", keycodeAsString(msg->msg.key));
            break;
        case M_Release:
            printf("Release key: %s\n", keycodeAsString(msg->msg.key));
            break;
        case M_PressFor:
            printf("Tap key: %s for %ims\n", keycodeAsString(msg->msg.key), msg->delay);
            break;
        case M_Delay:
            printf("Delay: %ims", msg->delay);
            break;
        case M_RepeatNext:
            printf("Repeat next message: %i times with a delay of %ims\n", msg->msg.count, msg->delay);
            break;
        case M_ServerClose:
            printf("ServerClose\n");
            break;
        default:
            printf("Un recognized Msg code %i\n", msg->type);
    }
}


void printSerialMsg(struct SerialMessage smsg) {
    struct SerialExtractor extractor = serialExtractor(&smsg);
    struct Message msg;
    printf("Compound Message:\n");
    int res;
    while ((res = extractMessage(&extractor, &msg))) {
        if (res == -1) {
            printf("Invalid Serial Message Format\n");
            return;
        }
        printMessage(&msg);
    }
}


void expandSerialMsg(struct SerialMessage *smsg, unsigned int needed) {
    if (smsg->capacity == 0){
        needed += 4;
    }
    unsigned int newCap = (smsg->capacity < 32)? 32 : smsg->capacity;
    while (newCap < needed) {newCap *= 2;}
    
    smsg->msgBuffer = realloc(smsg->msgBuffer, newCap);
    smsg->capacity = newCap;
}

// void addNullTerminator(struct SerialMessage *smsg) {
//     if (smsg->capacity < 4) {
//         expandCompoundMsg(smsg, 4);
//     }
//     memset(smsg->msgBuffer + smsg->msgLen, 0, 4);
// }



struct SerialMessage initSerialMsg() {
    struct SerialMessage smsg = {
        NULL,
        0,
        0
    }; 
    return smsg;
}

struct SerialMessage initSerialMsgCapacity(unsigned int capacity) {
    if (capacity < 4) capacity = 4;
    struct SerialMessage smsg = {
        .msgBuffer = malloc(capacity),
        .msgLen = 4,
        .capacity = capacity
    }; 
    return smsg;
}

struct SerialMessage initSerialMsgFrom(unsigned char *buffer, unsigned int capacity) {
    struct SerialMessage smsg = {
        buffer,
        0,
        capacity
    };
    return smsg;
}

void deinitSerialMsg(struct SerialMessage *smsg) {
    clearAndFree(smsg);
}

void clearRetainingCapacity(struct SerialMessage *smsg) {
    smsg->msgLen = 4; // Retain empty prefix
}

void clearAndFree(struct SerialMessage *smsg) {
    free(smsg->msgBuffer);
    smsg->msgBuffer = NULL;
    smsg->capacity = 0;
    smsg->msgLen = 0;
}

unsigned int serialFullLen(struct SerialMessage smsg) {
    if (smsg.msgBuffer == NULL) return 0;
    return smsg.msgLen;
}



// // Appending Message object

int assertLen(struct SerialMessage *smsg, unsigned int len, unsigned char isDynamic) {
    if (smsg->capacity - smsg->msgLen < len) {
        if (!isDynamic) return -1;
        else expandSerialMsg(smsg, smsg->msgLen + len);
    }
    return 0;
}

void appendType(struct SerialMessage *smsg, enum MessageType type) {
    uint32_t msgCode = type;
    *(uint32_t*)(smsg->msgBuffer + smsg->msgLen) = msgCode;
    smsg->msgLen += 4;
}

void appendStr(struct SerialMessage *smsg, const char *str, unsigned int strLen) {
    memcpy(smsg->msgBuffer+smsg->msgLen, str, strLen+1);
    smsg->msgLen += strLen+1;
}

void appendKeycode(struct SerialMessage *smsg, enum KeyCode key) {
    uint32_t u32keyCode = key;
    *(uint32_t*)(smsg->msgBuffer + smsg->msgLen) = u32keyCode;
    smsg->msgLen += 4;
}

void appendU32(struct SerialMessage *smsg, uint32_t value) {
    *(uint32_t*)(smsg->msgBuffer + smsg->msgLen) = value;
    smsg->msgLen += 4;
}

int serialMsgAppendWorker(struct SerialMessage *smsg, struct Message other, unsigned char isDynamic) {
    int stringLen;
    int msgSize;
    switch (other.type) {
        case M_Type:
            // <Type> <string>
            stringLen = strlen(other.msg.str);
            msgSize = 4+stringLen+1; // Msg type + strlen + null-terminal 
            if (assertLen(smsg, msgSize, isDynamic)) return -1;
            appendType(smsg, other.type);
            appendStr(smsg, other.msg.str, stringLen);
            break;
        case M_TypeDelay:
            // <Type> <delay> <string>
            stringLen = strlen(other.msg.str);
            msgSize = 8+stringLen+1; // Msg type + delay + strlen + null-terminal 
            if (assertLen(smsg, msgSize, isDynamic)) return -1;
            appendType(smsg, other.type);
            appendU32(smsg, other.delay);
            appendStr(smsg, other.msg.str, stringLen);
            break;
        case M_Press:
        case M_Hold:
        case M_Release:
            // <Type> <keycode>
            msgSize = 8; // Msg type + keycode 
            if (assertLen(smsg, msgSize, isDynamic)) return -1;
            appendType(smsg, other.type);
            appendU32(smsg, other.msg.key);
            break;
        case M_PressFor:
            // <Type> <delay> <keycode>
            msgSize = 12; // Msg type + delay + keycode 
            if (assertLen(smsg, msgSize, isDynamic)) return -1;
            appendType(smsg, other.type);
            appendU32(smsg, other.delay);
            appendU32(smsg, other.msg.key);
            break;
        case M_Delay:
            // <Type> <delay>
            msgSize = 8; // Msg type + delay 
            if (assertLen(smsg, msgSize, isDynamic)) return -1;
            appendType(smsg, other.type);
            appendU32(smsg, other.delay);
            break;
        case M_RepeatNext:
            // <Type> <count> <delay>
            msgSize = 12; // Msg type + count + delay 
            if (assertLen(smsg, msgSize, isDynamic)) return -1;
            appendType(smsg, other.type);
            appendU32(smsg, other.delay);
            break;
        case M_ServerClose:
            // <Type>
            msgSize = 4; // Msg type + count + delay 
            if (assertLen(smsg, msgSize, isDynamic)) return -1;
            appendType(smsg, other.type);
            break;
        default: fprintf(stderr, "Unknown message type encountered\n"); return -1;
    }
    return 0;
}

int serialMsgAppend(struct SerialMessage *smsg, struct Message other) {
    return serialMsgAppendWorker(smsg, other, 0); // Append without dynamically expanding memory
}

void dynamicSerialMsgAppend(struct SerialMessage *smsg, struct Message other) {
    serialMsgAppendWorker(smsg, other, 1); // Append while allowing dynamically expanding memory
}


// Appending serial message

int serialMsgAppendSerial(struct SerialMessage *smsg, struct SerialMessage other) {
    // Ensure cap is can fit other msg size + serial null terminator
    const unsigned int msgSize = (other.msgLen - 4); // -4 to account prefix space
    if (smsg->capacity - smsg->msgLen < msgSize) {
        return -1;
    }
    
    //                                                    v accounting for prefix space
    memcpy(smsg->msgBuffer + smsg->msgLen, other.msgBuffer+4, other.msgLen-4);
    smsg->msgLen += other.msgLen;
    return 0;
}

void dynamicSerialMsgAppendSerial(struct SerialMessage *smsg, struct SerialMessage other) {
    // Ensure cap is can fit other msg size + serial null terminator
    const unsigned int msgSize = (other.msgLen - 4); // -4 to account prefix space
    if (smsg->capacity - smsg->msgLen < msgSize) {
        expandSerialMsg(smsg, smsg->msgLen + msgSize);
    }
    
    //                                                    v accounting for prefix space
    memcpy(smsg->msgBuffer + smsg->msgLen, other.msgBuffer+4, other.msgLen-4);
    smsg->msgLen += other.msgLen;
}


// Message extraction functions

struct SerialExtractor serialExtractor(struct SerialMessage *smsg) {
    struct SerialExtractor extractor = {
        smsg,
        4 // After reserved prefix
    };
    return extractor;
}

enum MessageType readType(unsigned char *msgBuffer, unsigned int *pos) {
    uint32_t type32bit = *((uint32_t*)(msgBuffer + *pos)); // read as 32bit to ensure consistant byte count
    (*pos) += 4;
    return (enum MessageType)type32bit;
}

const char *readString(unsigned char *msgBuffer, unsigned int *pos) {
    const char *strPtr = msgBuffer+*pos;
    while (msgBuffer[*pos]) ++(*pos); // Move pos to after string
    ++(*pos);
    return strPtr;
}

enum KeyCode readKeyCode(unsigned char *msgBuffer, unsigned int *pos) {
    uint32_t type32bit = *((uint32_t*)(msgBuffer + *pos)); // read as 32bit to ensure consistant byte count
    (*pos) += 4;
    return (enum KeyCode)type32bit;
}

uint32_t readU32(unsigned char *msgBuffer, unsigned int *pos) {
    uint32_t type32bit = *((uint32_t*)(msgBuffer + *pos)); // read as 32bit to ensure consistant byte count
    (*pos) += 4;
    return type32bit;
}

int extractMessage(struct SerialExtractor *extractor, struct Message *msg) {
    if (extractor->pos >= extractor->smsg->msgLen) return 0;
    msg->type = readType(extractor->smsg->msgBuffer, &(extractor->pos));
    switch (msg->type) {
        case M_Type:
            // <Type> . <string>
            msg->msg.str = readString(extractor->smsg->msgBuffer, &(extractor->pos));
            break;
        case M_TypeDelay:
            // <Type> . <delay> <string>
            msg->delay = readU32(extractor->smsg->msgBuffer, &(extractor->pos));
            msg->msg.str = readString(extractor->smsg->msgBuffer, &(extractor->pos));
            break;
        case M_Press:
        case M_Hold:
        case M_Release:
            // <Type> . <keycode>
            msg->msg.key = readKeyCode(extractor->smsg->msgBuffer, &(extractor->pos));
            break;
        case M_PressFor:
            // <Type> . <delay> <keycode>
            msg->delay = readU32(extractor->smsg->msgBuffer, &(extractor->pos));
            msg->msg.key = readKeyCode(extractor->smsg->msgBuffer, &(extractor->pos));
            break;
        case M_Delay:
            // <Type> . <delay>
            msg->delay = readU32(extractor->smsg->msgBuffer, &(extractor->pos));
            break;
        case M_RepeatNext:
            // <Type> . <count> <delay>
            msg->msg.count = readU32(extractor->smsg->msgBuffer, &(extractor->pos));
            msg->delay = readU32(extractor->smsg->msgBuffer, &(extractor->pos));
            break;
        case M_ServerClose:
            // <Type> .
            break;
        default: fprintf(stderr, "Unknown message type encountered\n"); return -1;
    }
    return 1;
}
