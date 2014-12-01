#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <cstring>

#include "util.h"

using namespace std;

bool WriteUnix(int fileDescriptor, string text) {
	const char *c_text = text.c_str();
	size_t length = strlen(c_text);
	size_t bytesWritten = write(fileDescriptor, c_text, length);
	return bytesWritten == length;
}