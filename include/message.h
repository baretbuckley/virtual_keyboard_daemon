#ifndef MESSAGE_H
#define MESSAGE_H

#include "keycode.h"

enum MessageType {
    M_String = 0x01,
    M_Key = 0x02,
    M_ServerClose = 0x03,
};

// Serializes sequence of message to be <4 byte space> (<msg code>[msg value])*
// 4 byte space is reserved to use msg len prefix in inter process communication,
// it is undefined for any reference outside of that usecase
struct SerialMessage {
    unsigned char *msgBuffer;
    unsigned int msgLen; // Includes NULL prefix
    unsigned int capacity;
};

struct SerialExtractor {
    struct SerialMessage *smsg;
    unsigned int pos;
};

struct Message {
    enum MessageType type;
    union {
        const char * str;
        enum KeyCode key;
    } msg;
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


// non dynamic appends to serial message

// Appends server close signal to the serial message
// Returns -1 if buffer is out of space
int serialMsgAppendServerClose(struct SerialMessage *smsg);

// Appends string to the serial message
// given string is copied to internal buffer
// Returns -1 if buffer is out of space
int serialMsgAppendString(struct SerialMessage *smsg, const char* str);

// Appends key message to the serial message
// Returns -1 if buffer is out of space
int serialMsgAppendKey(struct SerialMessage *smsg, enum KeyCode key);

// Appends given message to the serial message object
// Returns -1 if buffer is out of space
int serialMsgAppend(struct SerialMessage *smsg, struct Message other);

// Appends serial messages. copies others internal buffer to smsg
// Returns -1 if buffer is ot of space
int serialMsgAppendSerial(struct SerialMessage *smsg, struct SerialMessage other);


// dynamic memory appending to serial message

// Appends server close signal to the serial message
// Dynamically grows internal buffer as need
void dynamicSerialMsgAppendServerClose(struct SerialMessage *smsg);

// Appends string to the serial message
// given string is copied to internal buffer
// Dynamically grows internal buffer as need
void dynamicSerialMsgAppendString(struct SerialMessage *smsg, const char* str);

// Appends key message to the serial message
// Dynamically grows internal buffer as need
void dynamicSerialMsgAppendKey(struct SerialMessage *smsg, enum KeyCode key);

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