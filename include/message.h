#ifndef MESSAGE_H
#define MESSAGE_H

#include "keycode.h"

#include <stdint.h>

enum MessageType {
    M_Type = 0x01,          // Type <string> 
    M_TypeDelay = 0x02,     // Type <string> with key delay of <u32> ms
    M_Press = 0x03,         // Press <key>
    M_Hold = 0x05,          // Press <key> and hold
    M_Release = 0x06,       // Release <key>
    M_PressFor = 0x04,      // Press <key> with a delay of <u32> ms before release
    M_Delay = 0x07,         // Delay execution for <u32> ms
    M_RepeatNext = 0x08,    // Repeat next command <u32> times with delay of <u32> ms between 
    M_ServerClose = 0x09,   // Close server
};

// Serializes sequence of messages as a byte stream
struct SerialMessage {
    unsigned char *msgBuffer;
    unsigned int msgLen;
    unsigned int capacity;
};

// Object wrapping around a serial message reference allowing messages
// to be deserialized
struct SerialExtractor {
    struct SerialMessage *smsg;
    unsigned int pos;
};

struct Message {
    union {
        const char * str;
        enum KeyCode key;
        uint32_t count;
    } msg;
    enum MessageType type;
    uint32_t delay;
};

// print message to stdout
void printMessage(struct Message *msg);

// Print messages from serail in order
void printSerialMsg(struct SerialMessage smsg);


// constructs Serial Message object, does not allocate buffer untill first append
struct SerialMessage initSerialMsg();

// Constructs Serial Message with given buffer capacity
struct SerialMessage initSerialMsgCapacity(unsigned int capacity);

// Constructs Serial Message using given buffer with capacity
// object takes ownership of buffer and may reallocate as needed
struct SerialMessage initSerialMsgFrom(unsigned char *buffer, unsigned int capacity);


// Deinits serial message, clearing internal buffer as needed
void deinitSerialMsg(struct SerialMessage *smsg);


// Clears serial object but retains buffer and capacity
void clearRetainingCapacity(struct SerialMessage *smsg);

// Clears Serial Object and frees internal buffer
void clearAndFree(struct SerialMessage *smsg);

// Get length of serial message not including the reserved 4 byte prefix
unsigned int serialMsgLen(struct SerialMessage smsg);

// non dynamic appends to serial message

// // Appends server close signal to the serial message
// // Returns -1 if buffer is out of space
// int serialMsgAppendServerClose(struct SerialMessage *smsg);

// // Appends string to the serial message
// // given string is copied to internal buffer
// // Returns -1 if buffer is out of space
// int serialMsgAppendType(struct SerialMessage *smsg, const char* str);

// // Appends key message to the serial message
// // Returns -1 if buffer is out of space
// int serialMsgAppendKey(struct SerialMessage *smsg, enum KeyCode key);

// Appends given message to the serial message object
// Returns -1 if buffer is out of space
int serialMsgAppend(struct SerialMessage *smsg, struct Message other);

// Appends serial messages. copies others internal buffer to smsg
// Returns -1 if buffer is ot of space
int serialMsgAppendSerial(struct SerialMessage *smsg, struct SerialMessage other);

// Apend M_Type message with a specified str length rather than null-terminated
// Returns -1 if the buffer is out of space
int serialMsgAppendTypeN(struct SerialMessage *smsg, const char* msgStr, unsigned int strLen);

// Apend M_TypeDelay message with a specified str length rather than null-terminated
// Returns -1 if the buffer is out of space
int serialMsgAppendTypeDelayN(struct SerialMessage *smsg, const char* msgStr, unsigned int delay, unsigned int strLen);


// dynamic memory appending to serial message

// Appends given message to the serial message object
// Dynamically grows internal buffer as need
void dynamicSerialMsgAppend(struct SerialMessage *smsg, struct Message other);

// Appends serial messages. copies others internal buffer to smsg
// Dynamically grows internal buffer as need
void dynamicSerialMsgAppendSerial(struct SerialMessage *smsg, struct SerialMessage other);



// Constructs serial extactor object for iterating through serial message
struct SerialExtractor serialExtractor(struct SerialMessage *smsg);

// Extract message and store in msg
// Any message with referenced data (i.e. string message) has lifetime equal to the SerialMessage it is exteracted from
int extractMessage(struct SerialExtractor *extractor, struct Message *msg);

#endif // MESSAGE_H