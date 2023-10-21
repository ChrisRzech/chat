NOTE: Outdated but kept just for reference.

# Goal
This document aims to define high-level and low-level design of the application.

# Features
The main goal is to create an application that allows users to communicate among each other.

Users must use a client that knows how to send data to and receive data from a server. Servers must be hosted on a dedicated server or ran
locally by an individual. Servers must never communicate with another server. From this point forwards, *the application* refers to the
logic behind the server unless stated otherwise.

Users may join a server by signing up to that server. Signing up must require a username-password credential which will be used to log in
into the account.

The application must offer users with different types of communication for users: private chat, group chat, and channel chat. A private
communication is between two users. A group communication is communication between two or more users.

Besides this, the application must provide other features.

# Server

A chat application allows users to communicate with each other. Any communication between users' clients must go through a server; clients
must never perform peer-to-peer communication. The server is the most important piece as it is the means of communication between
clients and persists data; therefore, the server must be a long-lived application that must not stop executing unless being told to stop or
a fatal error occurred. This document aims to describe the design of the server.

> Note: This design has no regards for a distributed system.

One of the simplest functionalities that the server needs to support is being able to send multiple messages to multiple clients for each
request that comes in.

For any incoming connection to the server, a thread should be spawned to handle the communication between the client and server. To do this,
an object of class `ClientConnection`, which holds a thread and the connected TCP socket, should be created. The interface for
`ClientConnection` includes `start()` and `stop()`. `ClientConnection::start()` should be a non-blocking function while
`ClientConnection::stop()` should be a blocking function. The socket should be in non-blocking mode to allow the thread to stop.

Idea:
Maybe the client should ask if there are any messages for it. Then, the server only returns messages that are unread. Well, maybe not
only unread since what if clients want to view old messages. For that, maybe clients should ask for messages between some datetime. Whatever
API the client needs to perform, making the client poll for updates instead of the server pushing updates might simplify things.

Note:
This section does not go into details about what exactly is communicated between the client and server.

The client connection should wait for any incoming packets and parse them into the appropriate message. The message should either be of type
`Request` or `Close`. If the message is a `Close`, the client connection should close the socket and end the thread execution.

Concern:
If a `ClientConnection` closes, shouldn't it be removed from the `ClientConnection` list the server holds? The design of this might have to
be redone. Is the list really needed? If you want to stop the server, the list is necessary otherwise you don't know who is still running.

Idea:
I'm starting to think the server should be pushing new messages out to clients. This would reduce the server load as clients wouldn't need
to constantly poll for more information. In addition, clients should be able to query for old messages. The connection should behave
bidirectionally where clients can send requests and servers can push information to the client. I suggest that there is a new message type
called "Push" which goes from the server to client. The server should not wait for an acknowledgement from the client since the server has
better things to do. However, this would mean that the threads would need to communicate with each other. Maybe a thread should have a queue
of messages that they need to push out where other threads can place messages into.

Concern:
The thread per client might affect performance. Since the sockets are not blocking (the thread cannot be blocked and they also need to send
and receive messages), the threads might essentially be busy-waiting. Therefore, the whole program will have multiple threads that are
busy-waiting thus slowing down performance. A faster solution would be to have a list of sockets that
