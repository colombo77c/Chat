/*-----------------------------------------------------------
---	Overview     ----------------------------------------
-----------------------------------------------------------*/

ChatMe is a chat program that allows people to send messages
over a local network or the internet. ChatMe is designed to 
be lightweight and extremely portable so that anyone can 
clone the repository and quickly set up their own server or 
connect to a friend's. With this in mind, ChatMe is built 
using only system calls for networking—avoiding dragging in
bulky libraries that may not work on all unix-type systems. 


/*-----------------------------------------------------------
---	Development  ----------------------------------------
-----------------------------------------------------------*/

This project was developed by Adam Colombo, Felipe Tulio, and
Jinshuo Dong. The work was divided as follows:

Adam Colombo - Server and Client classes
Felipe Tulio - Message class and password login
Jinshuo Dong - Color output, testing, style

/*-----------------------------------------------------------
---	Technical Accomplishments    ------------------------
-----------------------------------------------------------*/

Server - The biggest challenge while building the server was 
navigating the system calls required to bind to a port and 
listen for incoming connections. Another significant achievement
was building the server so that it handles connections on multiple
threads. This allows the server to keep connections open, making
the networking more efficient. Multithreading also allows the 
server to handle more simultaneous users than a signal thread
would be able to. 
	The server is also responsible for maintaining a map of 
users to sockets which is used when broadcasting messages. 
Another challenge was maintaining this mapping even after 
clients dropping connections and other network problems. On 
every round of broadcasting, the server checks the health of
each connection and if a socket is found to be closed or unresponsive,
the server will close that connection and remove the user from
the server. 

Client - One of the biggest challenges on the client side was 
being able to wait for user input while at the same time showing
live messages as they were being pushed down by the server.
The client also contains code for parsing messages as they are 
sent to the server and interpretting messages sent back from the 
server including handing any error messages received. 

Message - We originally looked at Google's Protocol Buffer 
library for message encoding; however, Protocol buffer's have no
support for sending a message over a TCP socket. This means we 
would have had to develop our own protocol for sending such 
buffers anyway. Additionally, we were sensitive to adding an
additional library which violates the lightweight ideal we are
striving towards. The Message Read & Write functions ensure that
the Server and Client classes never have to directly read from
a socket which makes it easy to test and ensure functionality.

User List - We decided to implement a username/password login to
keep track of the users that already logged in and the new ones.
Everytime someone tries to connect to the char, we first check
if the username was used before. If it is the case, we then check
the password. If the user wasn't in our files, we create a new one.


/*-----------------------------------------------------------
---	Features    -----------------------------------------
-----------------------------------------------------------*/

- User Authentication
- Different colors for each users
- Broadcast Messaging
- Private Messaging
- Stores previous messages for new chatters
- Multithreaded for better performance


/*-----------------------------------------------------------
---	Running    ------------------------------------------
-----------------------------------------------------------*/

To run, first launch the server by running:

./server 9000

Then launch a client by running:

./client 127.0.0.1 9000

Clients running on your local network or the internet can also 
connect to your server by replacing 127.0.0.1 with your public
ip address. 

/*-----------------------------------------------------------
---	Chatting    -----------------------------------------
-----------------------------------------------------------*/

Users can send broadcast messages by typing directly into their
terminal and pressing enter. Private messages can be sent by 
typing "private-[username]:[message]". For example, to send 
the message "Psst" to the user with username "Adam", a user
would type "private-Adam:Psst". To exit the chat, a user can
type “exit” and other users will be notified of their departure.





