##WireForge##

A custom TCP chat server built from the socket layer upward using C++ and POSIX sockets.

**What is WireForge?**

WireForge is a learning project where I am building a chat application from the ground up to understand how network communication actually works.

The project starts with basic TCP sockets and gradually builds more functionality on top of them.

The goal is to understand the path from:

TCP Sockets
    ↓
Client / Server Communication
    ↓
Multiple Clients
    ↓
Custom Chat Protocol
    ↓
Message Routing
    ↓
Reliable and Scalable Server

At the current stage, WireForge can:

Create a TCP server using POSIX sockets.
Listen for incoming connections on port 8080.
Accept multiple clients.
Keep client connections persistent.
Handle each connected client using a separate thread.
Receive messages from clients.
Echo messages back to the client that sent them.
Detect when a client disconnects.
Allow a client to quit the connection using quit.

**Technologies**
C++
POSIX Sockets
TCP
Linux
