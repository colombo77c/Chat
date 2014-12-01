#include <string>
#include <iostream>

#include "client.h"

using namespace std;

void PrintError(string errorMessage);
void PrintUsage();
void killHandler(int signal);

int main(int argc, char **argv) {
	int portNumber = -1;
	string hostName = "";

	if(argc < 3) {
		PrintError("Missing arguments");
	} else {
		hostName = argv[1];
		portNumber = atoi(argv[2]);
	}

	Client client(hostName, portNumber);
	try {
		client.Start();
	} catch(string exception) {
		cout << exception << endl;
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

void killHandler(int signal) {
	cout << "Shutting down the server ..." << endl;
}