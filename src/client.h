/*-----------------------------------------------------------

This file declares the Client class which is used to connect
to chat servers.
-----------------------------------------------------------*/


#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include <string>
#include <iostream>

using namespace std;

/*-----------------------------------------------------------
Connects to a chat server and presents a terminal-style 
interface for the user to enter messages. 

Example usage:

Client client("127.0.0.1", 9000);
client.Start();
-----------------------------------------------------------*/
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