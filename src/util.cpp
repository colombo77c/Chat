/*-----------------------------------------------------------

This file contains utility functions
-----------------------------------------------------------*/

#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <cstring>


using namespace std;


/*-----------------------------------------------------------
Writes a std::string to a file descriptor

@param fileDescriptor - the file descriptor to be written to

@param text - the std::string to be written

@return true if the string was successfully written completely,
	false otherwise
-----------------------------------------------------------*/
bool WriteUnix(int fileDescriptor, string text) {
	const char *c_text = text.c_str();
	size_t length = strlen(c_text);
	size_t bytesWritten = write(fileDescriptor, c_text, length);
	return bytesWritten == length;
}

/*-----------------------------------------------------------
Checks if the string starts with the given prefix

@param str - The string to check for prefix in

@param prefix - The prefix to check for

@return true if 'str' starts with 'prefix', false otherwise
-----------------------------------------------------------*/
bool StartsWith(string str, string prefix) {
	int result = str.compare(0, prefix.length(), prefix);
	return (result == 0);
}