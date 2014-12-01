#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <mutex>
#include <cstring>
#include <set>

#include "util.h"
#include "message.h"

using namespace std;

#define MAX_INCOMING_CONN_QUEUE_SIZE 25

typedef struct Connection {
	int socketDescriptor;
	Server *server;
} Connection;

Server::Server(int port) {
	m_port = port;
	m_numSavedMessages = 10;
}

void Server::SetNumSavedMessages(int num) {
	m_numSavedMessages = num;
}

int Server::GetNumSavedMessages() {
	return m_numSavedMessages;
}

void Server::Start() {
	/*********************************
	*** Creating socket file descriptor
	*** http://linux.die.net/man/2/socket
	**********************************/
	int serverSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if(serverSocketDescriptor < 0) {
		throw "Error creating server socket, please check your permissions";
	}

	/*********************************
	*** Binding socket to given port
	*** http://linux.die.net/man/2/bind
	*** http://man7.org/linux/man-pages/man7/ip.7.html
	**********************************/
	struct sockaddr_in serverSocket, clientSocket;
	serverSocket.sin_family = AF_INET;
	serverSocket.sin_addr.s_addr = INADDR_ANY;
	serverSocket.sin_port = htons(m_port);

	int bindResult = ::bind(serverSocketDescriptor, 
		(struct sockaddr *) &serverSocket,
		sizeof(serverSocket));
	if(bindResult < 0) {
		char *errorString = strerror(errno);
		throw errorString;
	}

	/*********************************
	*** Listen
	**********************************/
	int listenResult = listen(serverSocketDescriptor, MAX_INCOMING_CONN_QUEUE_SIZE);
	if(listenResult < 0) {
		char *errorString = strerror(errno);
		throw errorString;
	}

	/*********************************
	*** Accept Connections
	*** http://linux.die.net/man/2/accept
	**********************************/
	int acceptedSocketDescriptor;
	int clientSocketSize = sizeof(clientSocket);
	while((acceptedSocketDescriptor = accept(serverSocketDescriptor, (struct sockaddr *)&clientSocket, (socklen_t*) &clientSocketSize))) {
		
		
		pthread_t thread;

		Connection *conn = (Connection *)calloc(1, sizeof(Connection));
		conn->socketDescriptor = acceptedSocketDescriptor;
		conn->server = this;

		int threadResult = pthread_create(&thread, NULL, Server::HandleConnection, (void*) conn);
		if(threadResult < 0) {
			Message *error = new Message;
			error->SetType(SERVER_ERROR);
			error->SetBody("Sorry, there was an error handling your connection. Exiting now");
			Message::Write(error, acceptedSocketDescriptor);
			delete error;
		}
		
	}
}

void Server::Login(Message *message, int sendingDescriptor) {
	string username = message->GetBody();
	cout << "Logging in user " << username << " at descriptor " << sendingDescriptor << endl;
	m_chatMap[sendingDescriptor] = username;

	Message welcome;
	welcome.SetBody("Joined the chat!");
	welcome.SetType(LOGIN);
	BroadcastMessage(&welcome, sendingDescriptor);

	m_messagesLock.lock();
	for(vector<string>::reverse_iterator iter = m_pastMessages.rbegin(); iter != m_pastMessages.rend(); ++iter) {
		Message pastMessage;
		pastMessage.SetBody(*iter);
		pastMessage.Write(sendingDescriptor);
	}
	m_messagesLock.unlock();
	
}

void Server::Logout(Message *message, int sendingDescriptor) {
	string username = message->GetBody();
	cout << "Logging out user " << username << endl;

	Message exit;
	exit.SetBody("Left the chat!");
	exit.SetType(LOGOUT);
	BroadcastMessage(&exit, sendingDescriptor);
}


void Server::BroadcastMessage(Message *message, int sendingDescriptor) {
	string username = m_chatMap[sendingDescriptor];
	message->SetBody(username + ": " + message->GetBody());

	set<int> toBeDeleted;
	for(map<int, string>::iterator iter = m_chatMap.begin(); iter != m_chatMap.end(); iter++) {
		int key = iter->first;
		if(key != sendingDescriptor) {
			bool success = message->Write(key);
			if(!success) {
				toBeDeleted.insert(key);
			}
		}
	}
	for(set<int>::iterator iter = toBeDeleted.begin(); iter != toBeDeleted.end(); iter++) {
		m_chatMap.erase(*iter);
	}

	
	m_messagesLock.lock();
	if(m_pastMessages.size() >= GetNumSavedMessages()) {
		m_pastMessages.pop_back();
	}
	m_pastMessages.insert(m_pastMessages.begin(), message->GetBody());
	m_messagesLock.unlock();
	
	if(message->GetType() == LOGOUT) {
		map<int, string>::iterator it = m_chatMap.find(sendingDescriptor);
		m_chatMap.erase(it);
	}
}


void* Server::HandleConnection(void *data) {
	Connection *conn = (Connection *)data;
	int socketDescriptor = conn->socketDescriptor;
	Server *server = conn->server;

	bool isRunning = true;
	Message *receivedMessage;
	while(isRunning && (receivedMessage = Message::Read(socketDescriptor))) {
		MessageType type = receivedMessage->GetType();
		if(type == CLIENT_ERROR || type == SERVER_ERROR) {
			Message errorMessage;
			errorMessage.SetBody("There was an error with the previous request");
			errorMessage.Write(socketDescriptor);
		} else if(type == LOGIN) {
			server->Login(receivedMessage, socketDescriptor);
		} else if(type == LOGOUT) {
			server->Logout(receivedMessage, socketDescriptor);
			isRunning = false;
		} else {
			server->BroadcastMessage(receivedMessage, socketDescriptor);
		}
		delete receivedMessage;
	}

	cout << "Shutting down " << socketDescriptor << endl;
	shutdown(socketDescriptor, SHUT_RDWR);
	close(socketDescriptor);

	free(conn);

	pthread_exit(NULL);
}