/*-----------------------------------------------------------
acolombo, felipet, jinshuo
This file contains the declarations of utility functions
-----------------------------------------------------------*/

#ifndef CHAT_UTIL_H
#define CHAT_UTIL_H

#include <iostream>
#include <string>

using namespace std;

bool WriteUnix(int fileDescriptor, string text);

bool StartsWith(string str, string prefix);

#endif