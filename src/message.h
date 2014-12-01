#ifndef CHAT_MESSAGE_H
#define CHAT_MESSAGE_H

#include <string>
#include <iostream>

using namespace std;

#define MAX_BODY_LENGTH 1024
#define MAX_ERROR_LENGTH 1024


enum MessageType {
	LOGIN,
	LOGOUT,
	MESSAGE,
	SERVER_ERROR,
	CLIENT_ERROR
};

class Message {
private:
	string m_body;
	string m_error;
	bool m_hasError;
	MessageType m_type;

public:
	Message();

	string GetBody();
	void SetBody(string body);

	string GetError();
	void SetError(string error);
	bool HasError();

	MessageType GetType();
	void SetType(MessageType type);

	bool Write(int fileDescriptor);

	static bool Write(Message *message, int fileDescriptor);
	static Message* Read(int fileDescriptor);
};

#endif