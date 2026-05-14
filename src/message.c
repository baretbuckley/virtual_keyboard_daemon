#include <message.h>
#include <stdint.h>
#include <stdio.h>
#include <keycode.h>
#include <stdlib.h>
#include <string.h>

void printMessage(struct Message *msg) {
    switch (msg->type) {
        case M_ServerClose:
            printf("ServerClose\n");
            break;
        case M_String:
            printf("Type String: '%s'\n", msg->msg.str);
            break;
        case M_Key:
            printf("Tap key: %s\n", keycodeAsString(msg->msg.key));
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


// Appending server close message

// Worker function to do the append without bounds checking
void serialMsgAppendServerCloseWorker(struct SerialMessage *smsg) {
    uint32_t msgCode = M_ServerClose;
    *(uint32_t*)(smsg->msgBuffer + smsg->msgLen) = msgCode;
    // memcpy(smsg->msgBuffer+smsg->msgLen, &msgCode, 4);
    smsg->msgLen += 4;
}

int serialMsgAppendServerClose(struct SerialMessage *smsg) {
    // Ensure cap is can fit string (with null term byte) + serial null terminator
    const unsigned int msgSize = 4+4;
    if (smsg->capacity - smsg->msgLen < msgSize) {
        return -1;
    }
    serialMsgAppendServerCloseWorker(smsg);
    return 0;
}

void dynamicSerialMsgAppendServerClose(struct SerialMessage *smsg) {
    // Ensure cap is can fit string (with null term byte) + serial null terminator
    const unsigned int msgSize = 4;
    if (smsg->capacity - smsg->msgLen < msgSize) {
        expandSerialMsg(smsg, smsg->msgLen + msgSize);
    }
    serialMsgAppendServerCloseWorker(smsg);
}


// Appending string message

// Worker function to do the append without bounds checking
void serialMsgAppendStringWorker(struct SerialMessage *smsg, const char* str, int strLen) {
    uint32_t msgCode = M_String;
    *(uint32_t*)(smsg->msgBuffer + smsg->msgLen) = msgCode;
    smsg->msgLen += 4;
    memcpy(smsg->msgBuffer+smsg->msgLen, str, strLen+1);
    smsg->msgLen += strLen+1;
}

int serialMsgAppendString(struct SerialMessage *smsg, const char* str) {
    int strLen = strlen(str);
    // Ensure cap is can fit string + serial null terminator
    const unsigned int msgSize = 5+strLen;
    if (smsg->capacity - smsg->msgLen < msgSize) {
        return -1;
    }
    serialMsgAppendStringWorker(smsg, str, strLen);
    return 0;
}

void dynamicSerialMsgAppendString(struct SerialMessage *smsg, const char* str) {
    int strLen = strlen(str);
    // Ensure cap is can fit string + serial null terminator
    const unsigned int msgSize = 5+strLen;
    if (smsg->capacity - smsg->msgLen < msgSize) {
        expandSerialMsg(smsg, smsg->msgLen + msgSize);
    }
    serialMsgAppendStringWorker(smsg, str, strLen);
}


// Appending key message

// Worker function to do the append without bounds checking
void serialMsgAppendKeyWorker(struct SerialMessage *smsg, enum KeyCode key) {
    uint32_t msgCode = M_Key;
    *(uint32_t*)(smsg->msgBuffer + smsg->msgLen) = msgCode;
    smsg->msgLen += 4;

    uint32_t u32keyCode = key;
    *(uint32_t*)(smsg->msgBuffer + smsg->msgLen) = u32keyCode;
    smsg->msgLen += 4;
}

int serialMsgAppendKey(struct SerialMessage *smsg, enum KeyCode key) {
    // Ensure cap is can fit press key signa and keycode
    const unsigned int msgSize = 8;
    if (smsg->capacity - smsg->msgLen < msgSize) {
        return -1;
    }
    serialMsgAppendKeyWorker(smsg, key);
    return 0;
}

void dynamicSerialMsgAppendKey(struct SerialMessage *smsg, enum KeyCode key) {
    // Ensure cap is can fit press key signa and keycode
    const unsigned int msgSize = 8;
    if (smsg->capacity - smsg->msgLen < msgSize) {
        expandSerialMsg(smsg, smsg->msgLen + msgSize);
    }
    serialMsgAppendKeyWorker(smsg, key);
}


// Appending Message object

int serialMsgAppend(struct SerialMessage *smsg, struct Message other) {
    switch (other.type) {
        case M_ServerClose: return serialMsgAppendServerClose(smsg);
        case M_String: return serialMsgAppendString(smsg, other.msg.str);
        case M_Key: return serialMsgAppendKey(smsg, other.msg.key);
        default: fprintf(stderr, "Unknown message type encountered\n"); return -1;
    }
}

void dynamicSerialMsgAppend(struct SerialMessage *smsg, struct Message other) {
    switch (other.type) {
        case M_ServerClose: dynamicSerialMsgAppendServerClose(smsg); break;
        case M_String: dynamicSerialMsgAppendString(smsg, other.msg.str); break;
        case M_Key: dynamicSerialMsgAppendKey(smsg, other.msg.key); break;
    }
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
    const unsigned char *strPtr = msgBuffer+*pos;
    while (msgBuffer[*pos]) ++(*pos); // Move pos to after string
    ++(*pos);
    return strPtr;
}

enum KeyCode readKeyCode(unsigned char *msgBuffer, unsigned int *pos) {
    uint32_t type32bit = *((uint32_t*)(msgBuffer + *pos)); // read as 32bit to ensure consistant byte count
    (*pos) += 4;
    return (enum KeyCode)type32bit;
}

int extractMessage(struct SerialExtractor *extractor, struct Message *msg) {
    if (extractor->pos >= extractor->smsg->msgLen) return 0;
    msg->type = readType(extractor->smsg->msgBuffer, &(extractor->pos));
    switch (msg->type) {
        case M_ServerClose: break;
        case M_String: msg->msg.str = readString(extractor->smsg->msgBuffer, &(extractor->pos)); break;
        case M_Key: msg->msg.key = readKeyCode(extractor->smsg->msgBuffer, &(extractor->pos)); break;
        default: 
            printf("Failed to extract message type %i\n", msg->type); 
            return -1;
    }
    return 1;
}
