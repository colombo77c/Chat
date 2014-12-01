#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include <iostream>
#include <map>
#include <vector>
#include <mutex>

#include "message.h"

using namespace std;


class Server {
private:
	int m_port;
	map<int, string> m_chatMap;
	int m_numSavedMessages;
	vector<string> m_pastMessages;

	//std:: is required to compile on eniac. Who knows why
	std::mutex m_messagesLock;
	std::mutex m_mapLock;

	void Login(Message *m, int sendingDescriptor);
	void Logout(Message *m, int sendingDescriptor);
	void BroadcastMessage(Message *m, int sendingDescriptor);
	void HandleConn(Server *server, int fileDescriptor);
	static void *HandleConnection(void *data);
public:
	Server(int port);
	void Start();

	void SetNumSavedMessages(int num);
	int GetNumSavedMessages();
};



#endif