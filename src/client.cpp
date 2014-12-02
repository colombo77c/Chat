/*-----------------------------------------------------------

This file contains the implementation of the Client class 
which is used to connect to chat servers.
-----------------------------------------------------------*/
#include "client.h"

#include <stdio.h>
#include <stdexcept>
#include <stdlib.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

#include "util.h"
#include "message.h"

using namespace std;

const string SOCKET_EXCEPTION = "Error creating server socket, please check your permissions";
const string CONN_EXCEPTION = "Error connecting to server";
const string PRIVATE_PREFIX = "private-";
const int PRIVATE_PREFIX_LENGTH = 8;

/*-----------------------------------------------------------
Constructs a Client instance that will attempt to connect to a
server at 'host' on port 'port'.

@param host - The dotted quad ip address of the server the client
	should connect to

@param port - The port that the client should connect on
-----------------------------------------------------------*/
Client::Client(string host, int port) {
	m_hostName = host;
	m_port = port;
}


/*-----------------------------------------------------------
Starts the client by attempting to connect to the chat server
and starting the chat loop.
-----------------------------------------------------------*/
void Client::Start() {

	/*-----------------------------------------------------------
	Creating socket file descriptor
	See:
		http://linux.die.net/man/2/socket
	-----------------------------------------------------------*/
	int serverSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if(serverSocketDescriptor < 0) {
		throw runtime_error(SOCKET_EXCEPTION);
	}

	/*-----------------------------------------------------------
	Binding socket to given port
	See:
		http://linux.die.net/man/2/bind
		http://man7.org/linux/man-pages/man7/ip.7.html
	-----------------------------------------------------------*/
	struct sockaddr_in clientSocket;
	clientSocket.sin_family = AF_INET;
	clientSocket.sin_port = htons(m_port);
	inet_aton(m_hostName.c_str(), (in_addr *)&clientSocket.sin_addr.s_addr);

	int connectResult = connect(serverSocketDescriptor, (struct sockaddr *) &clientSocket, sizeof(clientSocket));
	if(connectResult < 0) {
		throw runtime_error(CONN_EXCEPTION);
	}

	m_socketDescriptor = serverSocketDescriptor;

	cout << "Connected to server ..." << endl;

	PromptLogin();
	StartChat();

	shutdown(serverSocketDescriptor, SHUT_RDWR);
	close(serverSocketDescriptor);
}

/*-----------------------------------------------------------
@param username - The username of the current user
-----------------------------------------------------------*/
void Client::SetUsername(string username) {
	m_username = username;
}


/*-----------------------------------------------------------
@return - The username of the current user
-----------------------------------------------------------*/
string Client::GetUsername() {
	return m_username;
}

/*-----------------------------------------------------------
Prompts the user to enter their username and collects their
input.
-----------------------------------------------------------*/
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

/*-----------------------------------------------------------
Starts the main chat loop. Listens for data both from the user's
input as well as incoming messages on the chat socket.
-----------------------------------------------------------*/
void Client::StartChat() {
	bool chatting = true;

	fd_set fileDescriptors;
	while(chatting) {
		cout << "> " << flush;

		/*-----------------------------------------------------------
		Create a file descriptor set to be used with the 'select' system
		call.
		-----------------------------------------------------------*/
		FD_ZERO(&fileDescriptors);
		FD_SET(STDIN_FILENO, &fileDescriptors);
		FD_SET(m_socketDescriptor, &fileDescriptors);

		/*-----------------------------------------------------------
		Listen for the first of either user input or new messages over
		the chat socket
		-----------------------------------------------------------*/
		int descriptor = select(m_socketDescriptor + 1, &fileDescriptors, NULL, NULL, NULL);
		if(descriptor < 0) {
			chatting = false;
			cout << "There was an error. Exiting" << endl;
		}

		/*-----------------------------------------------------------
		Determine the source of the input and respond accordingly
		-----------------------------------------------------------*/
		if(FD_ISSET(STDIN_FILENO, &fileDescriptors)) {
			chatting = HandleOutgoingMessage();
		} else {
			chatting = HandleReceivedMessage();
		}		
	}
}


/*-----------------------------------------------------------
Gets the user's input from STDIN, constructs a Message object
and writes that object to the socket. This function is called
when the user types a new message into their console.

@return true if the chat loop should continue and false if the 
	loop should exit
-----------------------------------------------------------*/
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
	} else if(StartsWith(message, PRIVATE_PREFIX)) {
		m->SetBody(message.substr(PRIVATE_PREFIX_LENGTH, string::npos));
		m->SetType(PRIVATE_MESSAGE);
	} else {
		m->SetType(BROADCAST_MESSAGE);
	}

	if(message != "") {
		m->Write(m_socketDescriptor);
	}
	return shouldContinue;
}

/*-----------------------------------------------------------
Gets a new message from the chat socket and writes the message
to STDOUT. This function is called when the user receives a new
message from the chat server.

@return true if the chat loop should continue and false if the 
	loop should exit
-----------------------------------------------------------*/
bool Client::HandleReceivedMessage() {
	Message *receivedMessage = Message::Read(m_socketDescriptor);
	cout << receivedMessage->GetBody() << endl;

	if(receivedMessage->GetType() == SERVER_ERROR) {
		return false;
	}
	return true;
}

