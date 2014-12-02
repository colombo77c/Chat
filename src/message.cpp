/*-----------------------------------------------------------

This file contains the implementation for the Message class. 

The most important methods included are Read and Write which 
handle deserialization and serialization of message instances 
respectively. 
-----------------------------------------------------------*/

#include "message.h"

#include <stdlib.h>
#include <unistd.h>
#include <sstream>

#include "util.h"


using namespace std;

/*-----------------------------------------------------------
The maximum number of bytes allowed to represent the length of
the message body.
-----------------------------------------------------------*/
#define MAX_BODY_HEADER 4

/*-----------------------------------------------------------
The default message body
-----------------------------------------------------------*/
#define DEFAULT_BODY ""

/*-----------------------------------------------------------
Constructs a Message object with default values
-----------------------------------------------------------*/
Message::Message() {
	SetBody(DEFAULT_BODY);
	SetType(MESSAGE);
}

/*-----------------------------------------------------------
@return the body of the message
-----------------------------------------------------------*/
string Message::GetBody() {
	return m_body;
}

/*-----------------------------------------------------------
Sets the message body

@param body - the message to be set
-----------------------------------------------------------*/
void Message::SetBody(string body) {
	m_body = body;
}

/*-----------------------------------------------------------
@return the type of the message
-----------------------------------------------------------*/
MessageType Message::GetType() {
	return m_type;
}

/*-----------------------------------------------------------
Sets the message type

@param type - the type to be set
-----------------------------------------------------------*/
void Message::SetType(MessageType type) {
	m_type = type;
}

/*-----------------------------------------------------------
Serialize a message and write to the given file descriptor

@param fileDescriptor - A file descriptor for the Message to
	be written to
-----------------------------------------------------------*/
bool Message::Write(int fileDescriptor) {
	stringstream builder;
	builder << GetBody().length() << ":" << GetBody();
	builder << GetType();
	builder << "\n";
	return WriteUnix(fileDescriptor, builder.str());
}


/*-----------------------------------------------------------
Attempts to read a new Message from the given file descriptor.
This function will block while waiting for data to be available.
-----------------------------------------------------------*/
Message* Message::Read(int fileDescriptor) {
	char buf[10];

	Message *m = new Message;

	/*-----------------------------------------------------------
	Getting body byte length
	-----------------------------------------------------------*/
	stringstream buffer;
	int bytesRead = 0;
	while(bytesRead < MAX_BODY_HEADER && buf[0] != ':') {
		int result = read(fileDescriptor, buf, 1);
		if(result < 0) {
			cout << "Error reading message" << endl;
			return NULL;
		}
		buffer << buf[0];
	}
	int bodyLength = atoi(buffer.str().c_str());
	if(bodyLength < 0 || bodyLength > MAX_BODY_LENGTH) {
		m->SetBody("Body length is invalid");
		m->SetType(SERVER_ERROR);
		return m;
	}

	/*-----------------------------------------------------------
	Reading the message body
	-----------------------------------------------------------*/
	string body(bodyLength, 0);
	int bodyBytesRead = read(fileDescriptor, &body[0], bodyLength);
	if(bodyBytesRead < bodyLength) {
		m->SetBody("Could not read the message body");
		m->SetType(SERVER_ERROR);
		return m;
	}
	m->SetBody(body);

	/*-----------------------------------------------------------
	Reading the message type
	-----------------------------------------------------------*/
	buffer.str("");
	bytesRead = read(fileDescriptor, buf, 1);
	if(bytesRead < 1) {
		m->SetBody("Error reading message type");
		m->SetType(SERVER_ERROR);
		return m;
	}
	buffer << buf[0];
	int messageType = atoi(buffer.str().c_str());

	//Casting encoded value to MessageType
	MessageType type = static_cast<MessageType>(messageType);
	m->SetType(type);


	/*-----------------------------------------------------------
	Read new line
	-----------------------------------------------------------*/
	read(fileDescriptor, buf, 1);

	return m;
}

/*-----------------------------------------------------------
Static version of Message::Write

@param m - Message to be written

@param fileDescriptor - file descriptor to be written to
-----------------------------------------------------------*/
bool Message::Write(Message *m, int fileDescriptor) {
	return m->Write(fileDescriptor);
}