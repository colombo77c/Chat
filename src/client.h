#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include <string>
#include <iostream>

using namespace std;


class Client {
private:
	int m_port;
	string m_hostName;
	int m_socketDescriptor;
	string m_username;

	string GetUsername();
	void SetUsername(string username);

	void PromptLogin();
	void StartChat();
	bool HandleReceivedMessage();
	bool HandleOutgoingMessage();
public:
	Client(string host, int port);
	void Start();
};

#endif