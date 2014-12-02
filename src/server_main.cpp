/*-----------------------------------------------------------
This file contains the main method for the server executable.
A port number is read from the program arguments and used to 
initialize a Server instance.
-----------------------------------------------------------*/

#include "server.h"

#include <string>
#include <iostream>


const string ERROR_MESSAGE = "There was an error starting the server: ";
Server *server = NULL;

void PrintError(string errorMessage);
void PrintUsage();

/*-----------------------------------------------------------
Declares a signal handling function used to gracefully shutdown
the server
-----------------------------------------------------------*/
void killHandler(int signal);


/*-----------------------------------------------------------
Main method of server executable. 
-----------------------------------------------------------*/
int main(int argc, char **argv) {
	int portNumber = -1;

	if(argc < 2) {
		PrintError("Must supply port number");
	} else {
		portNumber = atoi(argv[1]);
	}

	server = new Server(portNumber);
	try {
		//signal(SIGINT, killHandler);
		/*-----------------------------------------------------------
		Attempt to start the server
		-----------------------------------------------------------*/
		server->Start();
	} catch(exception& e) {

		cout << ERROR_MESSAGE << endl;
		cout << e.what() << endl;

	}
	
	delete server;
	cout << endl << "Shutting down the server ..." << endl;
}

void PrintError(string errorMessage) {
	cout << errorMessage << endl;
	PrintUsage();
	exit(1);
}

void PrintUsage() {
	cout << "server [port]" << endl;
}

/*-----------------------------------------------------------
Signal handler that gracefully shuts down the server.	
-----------------------------------------------------------*/
void killHandler(int signal) {
	server->Stop();
}