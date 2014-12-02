/*-----------------------------------------------------------
This file defines the Message class. Server and client instances
use Messages to communicate. The Message class also defines 
Read and Write functions for serialization/deserialization. 
-----------------------------------------------------------*/

#ifndef CHAT_MESSAGE_H
#define CHAT_MESSAGE_H

#include <string>
#include <iostream>

using namespace std;


/*-----------------------------------------------------------
The largest message body allowed
-----------------------------------------------------------*/
#define MAX_BODY_LENGTH 1024


/*-----------------------------------------------------------
MessageType describes what the message's purpose is
-----------------------------------------------------------*/
enum MessageType {
	LOGIN,
	LOGOUT,
	BROADCAST_MESSAGE,
	PRIVATE_MESSAGE,
	SERVER_ERROR,
	CLIENT_ERROR
};

/*-----------------------------------------------------------
Messages have a body and type and can be serialized/deserialized
in order to be sent over a socket. 
-----------------------------------------------------------*/
class Message {
private:
	string m_body;
	MessageType m_type;

public:
	Message();

	string GetBody();
	void SetBody(string body);

	MessageType GetType();
	void SetType(MessageType type);

	bool Write(int fileDescriptor);

	static bool Write(Message *message, int fileDescriptor);
	static Message* Read(int fileDescriptor);
};

#endif