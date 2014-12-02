/*-----------------------------------------------------------
This file contains the main method for the client executable.
A port number and host name are read from the program arguments 
and used to initialize a Client instance 
-----------------------------------------------------------*/

#include "client.h"

#include <string>
#include <iostream>


using namespace std;
const string ERROR_MESSAGE = "There was an error starting the client: ";

void PrintError(string errorMessage);
void PrintUsage();


/*-----------------------------------------------------------
Main method of client executable
-----------------------------------------------------------*/
int main(int argc, char **argv) {
	int portNumber = -1;
	string hostName = "";

	if(argc < 3) {
		PrintError("Missing arguments");
	} else {
		hostName = argv[1];
		portNumber = atoi(argv[2]);
	}


	/*-----------------------------------------------------------
	Create the client instance with the given host name and port.
	-----------------------------------------------------------*/
	Client client(hostName, portNumber);
	try {

		/*-----------------------------------------------------------
		Attempt to start the client
		-----------------------------------------------------------*/
		client.Start();
	} catch(exception& e) {
		cout << ERROR_MESSAGE << endl;
		cout << e.what() << endl;
	}
	
}

void PrintError(string errorMessage) {
	cout << errorMessage << endl;
	PrintUsage();
	exit(1);
}

void PrintUsage() {
	cout << "server [host] [port]" << endl;
}

