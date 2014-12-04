/*-----------------------------------------------------------
---	    Overview     ----------------------------------------
-----------------------------------------------------------*/

ChatMe is a chat program that allows people to send messages
over a local network or the internet. ChatMe is designed to 
be lightweight and extremely portable so that anyone can 
clone the repository and quickly set up their own server or 
connect to a friend's. With this in mind, ChatMe is built 
using only system calls for networking—avoiding dragging in
bulky libraries that may not work on all unix-type systems. 


/*-----------------------------------------------------------
---	    Development  ----------------------------------------
-----------------------------------------------------------*/

This project was developed by Adam Colombo, Felipe Tulio, and
Jinshuo Dong. The work was divided as follows:

Adam Colombo - Server and Client classes
Felipe Tulio - Message class and password login
Jinshuo Dong - Color output, testing

/*-----------------------------------------------------------
---	    Technical Accomplishments    ------------------------
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


Client - One of the biggest challenges on the client side was 
being able to wait for user input while at the same time showing
live messages as they were being pushed down by the server.

Message - We originally looked at Google's Protocol Buffer 
library for message encoding; however, Protocol buffer's have no
support for sending a message over a TCP socket. This means we 
would have had to develop our own protocol for sending such 
buffers anyway. Additionally, we were sensitive to adding an
additional library which violates the lightweight ideal we are
striving towards. 