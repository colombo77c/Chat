/*-----------------------------------------------------------

This file declares the Server class which is used to create a 
chat server for others to connect to.
-----------------------------------------------------------*/



#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include <iostream>
#include <map>
#include <vector>
#include <mutex>

#include "message.h"

using namespace std;

/*-----------------------------------------------------------
A chat server that others can connect to. Binds to 
localhost (127.0.0.1) at the port specified.

Example usage:

Server server(9000);
server.Start();
-----------------------------------------------------------*/
class Server {
private:
	int m_port;
	map<int, string> m_chatMap;
	map<string, int> m_reverseChatMap;
	int m_numSavedMessages;
	vector<string> m_pastMessages;

	//std:: is required to compile on eniac. Who knows why
	std::mutex m_messagesLock;
	std::mutex m_mapLock;

	void Login(Message *m, int sendingDescriptor);
	void Logout(Message *m, int sendingDescriptor);
	void BroadcastMessage(Message *m, int sendingDescriptor);
	void PrivateMessage(Message *m, int sendingDescriptor);
	bool HandleMessage(Message *m, int sendingDescriptor);
	void HandleConn(Server *server, int fileDescriptor);
	static void *HandleConnection(void *data);
public:
	Server(int port);
	void Start();

	void SetNumSavedMessages(int num);
	int GetNumSavedMessages();
};



#endif