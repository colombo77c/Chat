/*-----------------------------------------------------------

Contains the implementation of the Server class. The Server 
class uses system calls so readers should make sure to 
familiarize themselves with <unistd.h> <sys/socket.h> <pthread.h>
before continuing.

The Server class also makes extensive use of the Message class
as this class defines the chat protocol between Server and Client.

The Server is responsible for interpreting messages and responding
accordingly.
-----------------------------------------------------------*/

#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdexcept>
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


/*-----------------------------------------------------------
Connection struct used for passing information to newly spawned
connection handling threads. Contains an int socket descriptor
and a pointer to a Server object.
-----------------------------------------------------------*/
typedef struct Connection {
	int socketDescriptor;
	Server *server;
} Connection;

/*-----------------------------------------------------------
Declares a signal handling function used to gracefully shutdown
the server
-----------------------------------------------------------*/
void killHandler(int signal);


/*-----------------------------------------------------------
Constructs a Server instance that listens on the given port

@param port - The port to be listened on
-----------------------------------------------------------*/
Server::Server(int port) {
	m_port = port;
	m_numSavedMessages = 10;
}

/*-----------------------------------------------------------
Sets the number of messages that the server will save and
replay to new visitors.

@param num - The number of messages to be saved
-----------------------------------------------------------*/
void Server::SetNumSavedMessages(int num) {
	m_numSavedMessages = num;
}

/*-----------------------------------------------------------
Returns the number of messages that the server should save and
replay to new visitors.
-----------------------------------------------------------*/
int Server::GetNumSavedMessages() {
	return m_numSavedMessages;
}


/*-----------------------------------------------------------
Starts the server instance, listening on the port specified at
creation and 127.0.0.1. 

The specific steps in the process of starting the server are:
1. Creating socket file descriptor
2. Binding socket to the given port
3. Listen on socket
4. Accept connections on the given port and spawn handler threads

-----------------------------------------------------------*/
void Server::Start() {
	/*-----------------------------------------------------------
	Creating socket file descriptor
	See:
		http://linux.die.net/man/2/socket
	-----------------------------------------------------------*/
	int serverSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if(serverSocketDescriptor < 0) {
		string errorString = "Error creating server socket, please check your permissions";
		throw runtime_error(errorString);
	}

	/*-----------------------------------------------------------
	Binding socket to given port
	See:
		http://linux.die.net/man/2/bind
		http://man7.org/linux/man-pages/man7/ip.7.html
	-----------------------------------------------------------*/
	struct sockaddr_in serverSocket, clientSocket;
	serverSocket.sin_family = AF_INET;
	serverSocket.sin_addr.s_addr = INADDR_ANY;
	serverSocket.sin_port = htons(m_port);

	int bindResult = ::bind(serverSocketDescriptor, 
		(struct sockaddr *) &serverSocket,
		sizeof(serverSocket));
	if(bindResult < 0) {
		char *errorString = strerror(errno);
		throw runtime_error(errorString);
	}

	/*-----------------------------------------------------------
	Listen on socket
	-----------------------------------------------------------*/
	int listenResult = listen(serverSocketDescriptor, MAX_INCOMING_CONN_QUEUE_SIZE);
	if(listenResult < 0) {
		char *errorString = strerror(errno);
		throw runtime_error(errorString);
	}

	/*-----------------------------------------------------------
	Accept Connections
	See:
		http://linux.die.net/man/2/accept
	-----------------------------------------------------------*/
	int acceptedSocketDescriptor;
	int clientSocketSize = sizeof(clientSocket);
	while((acceptedSocketDescriptor = 
		accept(serverSocketDescriptor, 
			(struct sockaddr *)&clientSocket, 
			(socklen_t*) &clientSocketSize))) {
		
		
		pthread_t thread;

		Connection *conn = (Connection *)calloc(1, sizeof(Connection));
		conn->socketDescriptor = acceptedSocketDescriptor;
		conn->server = this;

		int threadResult = pthread_create(&thread, 
			NULL, 
			Server::HandleConnection, 
			(void*) conn);
		if(threadResult < 0) {
			Message *error = new Message;
			error->SetType(SERVER_ERROR);
			error->SetBody("Sorry, there was an error handling your connection. Exiting now");
			Message::Write(error, acceptedSocketDescriptor);
			delete error;
		}
		
	}
}

/*-----------------------------------------------------------
Handles a login message by adding the user's socket descriptor
and username to a map (key: sendingDescriptor; value: username)
and broadcasting the user's arrival to other connected users.

@param message - Login message. MessageType must be LOGIN and body must
	contain the username of the user to be logged in.

@param sendingDescriptor - File descriptor that the user is 
	connected to the server on.
-----------------------------------------------------------*/
void Server::Login(Message *message, int sendingDescriptor) {
	string username = message->GetBody();
	cout << "Logging in user ";
	cout << username << " at descriptor ";
	cout << sendingDescriptor << endl;

	m_chatMap[sendingDescriptor] = username;

	Message welcome;
	welcome.SetBody("Joined the chat!");
	welcome.SetType(LOGIN);
	BroadcastMessage(&welcome, sendingDescriptor);

	m_messagesLock.lock();
	for(vector<string>::reverse_iterator iter = m_pastMessages.rbegin(); 
		iter != m_pastMessages.rend(); 
		++iter) {
		Message pastMessage;
		pastMessage.SetBody(*iter);
		pastMessage.Write(sendingDescriptor);
	}
	m_messagesLock.unlock();
	
}

/*-----------------------------------------------------------
Handles a logout message by broadcasting a message announcing
the user's departure from the chat.

@param message - The logout message. MessageType must be LOGOUT
	and body must contain the username of the user to be logged
	out.

@param sendingDescriptor - File descriptor that the user is 
	connected to the server on.
-----------------------------------------------------------*/
void Server::Logout(Message *message, int sendingDescriptor) {
	string username = message->GetBody();
	cout << "Logging out user " << username << endl;

	Message exit;
	exit.SetBody("Left the chat!");
	exit.SetType(LOGOUT);
	BroadcastMessage(&exit, sendingDescriptor);
}

/*-----------------------------------------------------------
Handles broadcasting a message to all connected users. Writes 
the message body prepended by 'username: ' to every user except
the original sender.

@param message - The message to be broadcast. If the MessageType
	is LOGOUT, the sendingDescriptor will be removed from the 
	chat map.

@param sendingDescriptor - File descriptor that the user is 
	connected to the server on.
-----------------------------------------------------------*/
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

/*-----------------------------------------------------------
Handles the routing of messages to specific functions depending
on the MessageType. 

@param message - The message to be routed

@param sendingDescriptor - File descriptor that the user is 
	connected to the server on.

@return true if the calling thread should continue the chat loop,
	false if it should exit
-----------------------------------------------------------*/
bool Server::HandleMessage(Message *message, int sendingDescriptor) {
	MessageType type = message->GetType();
	if(type == CLIENT_ERROR || type == SERVER_ERROR) {
		Message errorMessage;
		errorMessage.SetBody("There was an error with the previous request");
		errorMessage.Write(sendingDescriptor);
	} else if(type == LOGIN) {
		Login(message, sendingDescriptor);
	} else if(type == LOGOUT) {
		Logout(message, sendingDescriptor);
		return false;
	} else {
		BroadcastMessage(message, sendingDescriptor);
	}
	return true;
}

/*-----------------------------------------------------------
This function is called by newly spawned connection handling 
threads. Runs the chat loop for one connection on its own thread.

@param data - Pointer to a Connection object. Must be declared as
	void * to conform to pthread library.
-----------------------------------------------------------*/
void* Server::HandleConnection(void *data) {
	Connection *conn = (Connection *)data;
	int socketDescriptor = conn->socketDescriptor;
	Server *server = conn->server;

	bool isRunning = true;
	Message *receivedMessage;

	/*-----------------------------------------------------------
	Main loop of chat program. Listen for new Message's on this
	connections socket and 
	-----------------------------------------------------------*/
	while(isRunning && (receivedMessage = Message::Read(socketDescriptor))) {
		isRunning = server->HandleMessage(receivedMessage, socketDescriptor);
		delete receivedMessage;
	}

	cout << "Shutting down " << socketDescriptor << endl;

	/*-----------------------------------------------------------
	Once a particular user has exited the chat, shut down socket
	and exit thread
	-----------------------------------------------------------*/
	shutdown(socketDescriptor, SHUT_RDWR);
	close(socketDescriptor);

	free(conn);

	pthread_exit(NULL);
}


/*-----------------------------------------------------------
Signal handler that gracefully shuts down the server.	
-----------------------------------------------------------*/
void killHandler(int signal) {

}