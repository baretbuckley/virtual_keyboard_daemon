#ifndef MESSAGE_H
#define MESSAGE_H

#include "keycode.h"

enum MessageType {
    M_NULL = 0x00, // For internal use
    M_String = 0x01,
    M_Key = 0x02,
    M_ServerClose = 0x03,
};

// Serializes sequence of message to be (<msg code>[msg value])* <4 byte null term>
struct SerialMessage {
    unsigned char *msgBuffer;
    unsigned int msgLen; // Does not include 4 byte null terminating signal used serialFullLen(struct SerialMessage)
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


// Get full length of serialized message including ending null terminal
// If internal buffer is NULL the full length is 0
unsigned int serialFullLen(struct SerialMessage smsg);


// Appends server close signal to the serial message
// Returns -1 if buffer is out of space
int serialMsgAppendServerClose(struct SerialMessage *smsg);

// Appends server close signal to the serial message
// Dynamically grows internal buffer as need
void dynamicSerialMsgAppendServerClose(struct SerialMessage *smsg);


// Appends string to the serial message
// serial object does not take ownership of string and copys it to internal buffer
int serialMsgAppendString(struct SerialMessage *smsg, const char* str);

// Appends key message to the serial message
int serialMsgAppendKey(struct SerialMessage *smsg, enum KeyCode key);

// Appends given message to the serial message object
int serialMsgAppend(struct SerialMessage *smsg, struct Message other);

// Appends serial messages. copies others internal buffer to smsg
void serialMsgAppendSerial(struct SerialMessage *smsg, struct SerialMessage other);


// Constructs serial extactor object for iterating through serial message
struct SerialExtractor serialExtractor(struct SerialMessage *smsg);

// Extract message and store in msg
int extractMessage(struct SerialExtractor *extractor, struct Message *msg);

#endif // MESSAGE_H