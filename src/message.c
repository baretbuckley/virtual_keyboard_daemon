#include <message.h>
#include <stdint.h>
#include <stdio.h>
#include <keycode.h>


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


void expandCompoundMsg(struct SerialMessage *smsg, unsigned int needed) {
    unsigned int newCap = (smsg->capacity < 32)? 32 : smsg->capacity;
    while (newCap < needed) {newCap *= 2;}
    
    smsg->msgBuffer = realloc(smsg->msgBuffer, newCap);
    smsg->capacity = newCap;
}

void addNullTerminator(struct SerialMessage *smsg) {
    if (smsg->capacity < 4) {
        expandCompoundMsg(smsg, 4);
    }
    memset(smsg->msgBuffer + smsg->msgLen, 0, 4);
}



struct SerialMessage initSerialMsg() {
    struct SerialMessage smsg = {
        NULL,
        0,
        0
    }; 
    return smsg;
}

struct SerialMessage initSerialMsgCapacity(unsigned int capacity) {
    struct SerialMessage smsg = {
        malloc(capacity),
        0,
        capacity
    }; 
    addNullTerminator(&smsg);
    return smsg;
}

struct SerialMessage initSerialMsgFrom(unsigned char *buffer, unsigned int capacity) {
    struct SerialMessage smsg = {
        buffer,
        0,
        capacity
    }; 
    addNullTerminator(&smsg);
    return smsg;
}

void deinitSerialMsg(struct SerialMessage *smsg) {
    clearAndFree(smsg);
}

void clearRetainingCapacity(struct SerialMessage *smsg) {
    smsg->msgLen = 0;
    addNullTerminator(smsg);
}

void clearAndFree(struct SerialMessage *smsg) {
    free(smsg->msgBuffer);
    smsg->msgBuffer = NULL;
    smsg->capacity = 0;
    smsg->msgLen = 0;
}

unsigned int serialFullLen(struct SerialMessage smsg) {
    if (smsg.msgBuffer == NULL) return 0;
    return smsg.msgLen += 4;
}


void serialMsgAppendServerCloseWorker(struct SerialMessage *smsg) {
    uint32_t msgCode = M_ServerClose;
    memcpy(smsg->msgBuffer+smsg->msgLen, &msgCode, 4);
    smsg->msgLen += 4;
    addNullTerminator(smsg);
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
    const unsigned int msgSize = 4+4;
    if (smsg->capacity - smsg->msgLen < msgSize) {
        expandCompoundMsg(smsg, smsg->msgLen + msgSize);
    }
    serialMsgAppendServerCloseWorker(smsg);
}


void serialMsgAppendStringWorker(struct SerialMessage *smsg, const char* str, int strLen) {
    uint32_t msgCode = M_String;
    memcpy(smsg->msgBuffer+smsg->msgLen, &msgCode, 4);
    smsg->msgLen += 4;
    memcpy(smsg->msgBuffer+smsg->msgLen, str, strLen+1);
    smsg->msgLen += strLen+1;
    addNullTerminator(smsg);
}

int serialMsgAppendString(struct SerialMessage *smsg, const char* str) {
    int strLen = strlen(str);
    // Ensure cap is can fit string (with null term byte) + serial null terminator
    const unsigned int msgSize = 5+strLen + 4;
    if (smsg->capacity - smsg->msgLen < msgSize) {
        return -1;
    }
    serialMsgAppendStringWorker(smsg, str, strLen);
    return 0;
}

void dynamicSerialMsgAppendString(struct SerialMessage *smsg, const char* str) {
    int strLen = strlen(str);
    // Ensure cap is can fit string (with null term byte) + serial null terminator
    const unsigned int msgSize = 5+strLen + 4;
    if (smsg->capacity - smsg->msgLen < msgSize) {
        expandCompoundMsg(smsg, smsg->msgLen + msgSize);
    }
    serialMsgAppendStringWorker(smsg, str, strLen);
}


void serialMsgAppendKeyWorker(struct SerialMessage *smsg, enum KeyCode key) {
    uint32_t msgCode = M_Key;
    memcpy(smsg->msgBuffer+smsg->msgLen, &msgCode, 4);
    smsg->msgLen += 4;

    uint32_t u32keyCode = key;
    memcpy(smsg->msgBuffer+smsg->msgLen, &u32keyCode, 4);
    smsg->msgLen += 4;
    addNullTerminator(smsg);
}

int serialMsgAppendKey(struct SerialMessage *smsg, enum KeyCode key) {
    // Ensure cap is can fit press key signa and keycode + serial null terminator
    const unsigned int msgSize = 8+4;
    if (smsg->capacity - smsg->msgLen < msgSize) {
        return -1;
    }
    serialMsgAppendKeyWorker(smsg, key);
    return 0;
}

void dynamicSerialMsgAppendKey(struct SerialMessage *smsg, enum KeyCode key) {
    // Ensure cap is can fit press key signa and keycode + serial null terminator
    const unsigned int msgSize = 8+4;
    if (smsg->capacity - smsg->msgLen < msgSize) {
        expandCompoundMsg(smsg, smsg->msgLen + msgSize);
    }
    serialMsgAppendKeyWorker(smsg, key);
}



void serialMsgAppend(struct SerialMessage *smsg, struct Message other) {
    switch (other.type) {
        case M_NULL: break;
        case M_ServerClose: serialMsgAppendServerClose(smsg); break;
        case M_String: serialMsgAppendString(smsg, other.msg.str); break;
        case M_Key: serialMsgAppendKey(smsg, other.msg.key); break;
    }
}

void serialMsgAppendSerial(struct SerialMessage *smsg, struct SerialMessage other) {
    // Ensure cap is can fit other msg size + serial null terminator
    const unsigned int msgSize = (other.capacity - other.msgLen)+4;
    if (smsg->capacity - smsg->msgLen < msgSize) {
        expandCompoundMsg(smsg, smsg->msgLen + msgSize);
    }
    
    memcpy(smsg->msgBuffer + smsg->msgLen, other.msgBuffer, other.msgLen);
    smsg->msgLen += other.msgLen;
    addNullTerminator(smsg);
}

struct SerialExtractor serialExtractor(struct SerialMessage *smsg) {
    struct SerialExtractor extractor = {
        smsg,
        0
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
    while (msgBuffer[*pos]) ++(*pos);
    ++(*pos);
    return strPtr;
}

enum KeyCode readKeyCode(unsigned char *msgBuffer, unsigned int *pos) {
    uint32_t type32bit = *((uint32_t*)(msgBuffer + *pos)); // read as 32bit to ensure consistant byte count
    (*pos) += 4;
    return (enum KeyCode)type32bit;
}

int extractMessage(struct SerialExtractor *extractor, struct Message *msg) {
    if (extractor->pos >= extractor->smsg->msgLen+4) return -1;
    msg->type = readType(extractor->smsg->msgBuffer, &(extractor->pos));
    switch (msg->type) {
        case M_NULL: return 0;
        case M_ServerClose: break;
        case M_String: msg->msg.str = readString(extractor->smsg->msgBuffer, &(extractor->pos)); break;
        case M_Key: msg->msg.key = readKeyCode(extractor->smsg->msgBuffer, &(extractor->pos)); break;
        default: 
            printf("Failed to extract message type %i\n", msg->type); 
            return -1;
    }
    return 1;
}

// // Serializes message writing to the given buffer. if message has type compound,
// // null ptr can be passed as buffer and it will return the internal buffer of the compound message.
// // If serialization fails due to buffer overflow or null buffer for non compound msg, return will be null
// unsigned char *serializeMessage(struct Message msg, unsigned char *buffer, unsigned int bufferLen, unsigned int *used) {
//     if (buffer == NULL && msg.type != M_Compound) return NULL;
//     switch (msg.type){
//         case M_Compound:
//             // If buffer is not provided, just return the buffer of the compound msg
//             // it is expected to be formated correctly and be null terminated
//             // Otherwise copy the compound message buffer to the given buffer.
//             // TODO: add validation step for confirming that the msgBuffer is correct
//             *used = msg.msg.compound.msgLen;
//             if (buffer == NULL) return msg.msg.compound.msgBuffer;
//             if (msg.msg.compound.msgLen > bufferLen) return NULL; // Fail due to buffer overflow
//             memcpy(buffer, msg.msg.compound.msgBuffer, msg.msg.compound.msgLen);
//             return buffer;
//         case M_Key:
//             // fill buffer in following format <key signal><key code><null terminal (4 bytes)>,.
//             if (bufferLen < 8+4) return NULL;
//             uint32_t msgCode = M_Key;
//             memcpy(buffer, &msgCode, 4);
//             uint32_t u32keyCode = msg.msg.key;
//             memcpy(buffer+sizeof(uint32_t), &u32keyCode, 4);
//             *used = 8;
//             break; // Adding msg null terminal after case stmt
//         case M_ServerClose:
//             // Fill buffer with following format <ServerClose signal><null terminal (4 bytes)>
//             if (bufferLen < 4+4) return NULL;
//             uint32_t msgCode = M_ServerClose;
//             memcpy(buffer, &msgCode, 4);
//             *used = 4;
//             break; // Adding msg null terminal after case stmt
//         case M_String:
//             // Fill buffer in following format <Type String Signal><null terminated str><null terminal (4 bytes)>
//             int strLen = strlen(msg.msg.str);
//             if (bufferLen < strLen+5+4) return NULL;
//             uint32_t msgCode = M_String;
//             memcpy(buffer, &msgCode, 4);
//             memcpy(buffer+sizeof(uint32_t), msg.msg.str, strLen+1);
//             *used = 4+strLen+1;
//             break; // Adding msg null terminal after case stmt
//         default:
//             return NULL;
//     }
//     memset(buffer+*used, 0, 4);
//     *used += 4;
//     return buffer;
// }