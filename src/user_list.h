/*-----------------------------------------------------------

This file declares the UserList class, that store every user
that connects to the server
-----------------------------------------------------------*/

#ifndef CHAT_USER_LIST_H
#define CHAT_USER_LIST_H

#include <iostream>
#include <map>

using namespace std;

class UserList {
private:
  
        map<string, string> m_userList;

public:
        UserList();
	bool UserRegistered(string username);
	bool CheckUserPassword(string username, string password);
	void AddUser(string username, string password);
	
	int GetNumUsers();

};


#endif
