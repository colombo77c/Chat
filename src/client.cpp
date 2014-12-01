#include "client.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

#include "message.h"

using namespace std;

Client::Client(string host, int port) {
	m_hostName = host;
	m_port = port;
}

void Client::Start() {
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
	struct sockaddr_in clientSocket;
	clientSocket.sin_family = AF_INET;
	clientSocket.sin_port = htons(m_port);
	inet_aton(m_hostName.c_str(), (in_addr *)&clientSocket.sin_addr.s_addr);

	int connectResult = connect(serverSocketDescriptor, (struct sockaddr *) &clientSocket, sizeof(clientSocket));
	if(connectResult < 0) {
		throw "Error connecting to server";
	}

	m_socketDescriptor = serverSocketDescriptor;

	PromptLogin();
	StartChat();

	shutdown(serverSocketDescriptor, SHUT_RDWR);
	close(serverSocketDescriptor);
}

void Client::SetUsername(string username) {
	m_username = username;
}

string Client::GetUsername() {
	return m_username;
}

void Client::PromptLogin() {
	cout << "Please enter your username: ";
	string username;
	cin >> username;

	cout << "Welcome to the chat room " << username << endl;

	SetUsername(username);

	Message *login = new Message;
	login->SetType(LOGIN);
	login->SetBody(username);
	login->Write(m_socketDescriptor);
}

void Client::StartChat() {
	bool chatting = true;

	fd_set fileDescriptors;
	while(chatting) {
		cout << "> " << flush;
		FD_ZERO(&fileDescriptors);
		FD_SET(STDIN_FILENO, &fileDescriptors);
		FD_SET(m_socketDescriptor, &fileDescriptors);

		int descriptor = select(m_socketDescriptor + 1, &fileDescriptors, NULL, NULL, NULL);
		if(descriptor < 0) {
			chatting = false;
			cout << "There was an error. Exiting" << endl;
		}

		if(FD_ISSET(STDIN_FILENO, &fileDescriptors)) {
			chatting = HandleOutgoingMessage();
		} else {
			chatting = HandleReceivedMessage();
		}		
	}
}

bool Client::HandleOutgoingMessage() {
	string message;
	getline(cin, message);

	Message *m = new Message;
	m->SetBody(message);

	bool shouldContinue = true;
	if(message == "exit") {
		m->SetBody(GetUsername());
		m->SetType(LOGOUT);
		shouldContinue = false;
	} else {
		m->SetType(MESSAGE);
	}

	if(message != "") {
		m->Write(m_socketDescriptor);
	}
	return shouldContinue;
}

bool Client::HandleReceivedMessage() {
	Message *receivedMessage = Message::Read(m_socketDescriptor);
	cout << receivedMessage->GetBody() << endl;

	if(receivedMessage->GetType() == SERVER_ERROR) {
		return false;
	}
	return true;
}

