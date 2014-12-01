#include <string>
#include <iostream>

#include "server.h"

void PrintError(string errorMessage);
void PrintUsage();

int main(int argc, char **argv) {
	int portNumber = -1;

	if(argc < 2) {
		PrintError("Must supply port number");
	} else {
		portNumber = atoi(argv[1]);
	}

	Server server(portNumber);
	server.Start();
}

void PrintError(string errorMessage) {
	cout << errorMessage << endl;
	PrintUsage();
	exit(1);
}

void PrintUsage() {
	cout << "server [port]" << endl;
}