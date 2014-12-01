#include "message.h"

#include <stdlib.h>
#include <unistd.h>
#include <sstream>

#include "util.h"


using namespace std;

#define MAX_BODY_HEADER 4

Message::Message() {
	SetBody("");
	SetError("");
	SetType(MESSAGE);
}

/************************
*** Body getter and setter
**************************/
string Message::GetBody() {
	return m_body;
}

void Message::SetBody(string body) {
	m_body = body;
}

/************************
*** Error getter and setter
**************************/
string Message::GetError() {
	return m_error;
}

void Message::SetError(string error) {
	m_hasError = (error != "");
	m_error = error;
}

bool Message::HasError() {
	return m_hasError;
}

/************************
*** Message type getter and setter
**************************/
MessageType Message::GetType() {
	return m_type;
}

void Message::SetType(MessageType type) {
	m_type = type;
}

/************************
*** Unix read/write from file descriptor
**************************/

bool Message::Write(int fileDescriptor) {
	stringstream builder;
	builder << GetBody().length() << ":" << GetBody();
	builder << GetType();
	builder << "\n";
	return WriteUnix(fileDescriptor, builder.str());
}

Message* Message::Read(int fileDescriptor) {
	char buf[10];

	Message *m = new Message;

	/*****************************
	*** Getting body byte length
	******************************/
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
		m->SetError("Body length is invalid");
		m->SetType(SERVER_ERROR);
		return m;
	}

	/*****************************
	*** Reading the message body
	******************************/
	string body(bodyLength, 0);
	int bodyBytesRead = read(fileDescriptor, &body[0], bodyLength);
	if(bodyBytesRead < bodyLength) {
		m->SetError("Could not read the message body");
		m->SetType(SERVER_ERROR);
		return m;
	}
	m->SetBody(body);

	/*****************************
	*** Reading the message type
	******************************/
	buffer.str("");
	bytesRead = read(fileDescriptor, buf, 1);
	if(bytesRead < 1) {
		m->SetError("Error reading message type");
		m->SetType(SERVER_ERROR);
		return m;
	}
	buffer << buf[0];
	int messageType = atoi(buffer.str().c_str());
	//cout << "Found type: " << messageType << endl;
	MessageType type = static_cast<MessageType>(messageType);
	m->SetType(type);


	/*******************************
	*** Read new line
	********************************/
	read(fileDescriptor, buf, 1);

	return m;
}

bool Message::Write(Message *m, int fileDescriptor) {
	return m->Write(fileDescriptor);
}