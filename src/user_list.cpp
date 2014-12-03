/*-----------------------------------------------------------

This file contains the implementation for the UserList class. 

It basically checks if the user is already registered in the
server, log the user in or create a new account.
-----------------------------------------------------------*/

#include "user_list.h"

#include <map>

using namespace std;

/*-----------------------------------------------------------
Constructs a UserList object.
-----------------------------------------------------------*/
UserList::UserList() { }

/*-----------------------------------------------------------
Check if the user was already registered

@param username - username to be checked

@return if the user is in the map or not
-----------------------------------------------------------*/
bool UserList::UserRegistered(string username) {
        map<string, string>::iterator it;
  
	it = m_userList.find(username);
	if (it == m_userList.end())
	        return false;
	else
	        return true;
}

/*-----------------------------------------------------------
Check if the password given is correct

@param username - username to be checked

@param password - password to be checked

@return if combination is correct or not
-----------------------------------------------------------*/
bool UserList::CheckUserPassword(string username, string password) {
        if (m_userList[username ] == password)
                return true;
	else
	        return false;
}

/*-----------------------------------------------------------
Insert user into the map

@param username - username to be inserted

@param password - user's password
-----------------------------------------------------------*/
void UserList::AddUser(string username, string password) {
        m_userList[username] = password;
}

/*-----------------------------------------------------------
Return the number of users registered in the server

@return the number of users
-----------------------------------------------------------*/
int UserList::GetNumUsers() {
        return m_userList.size();
}

