# CPP Chat App

This chat application that has 2 executables: a client and a server. They can exchange messages, with the receiving side always sending back and ACK, and the sending side printing out the round-trip latency in microseconds. We use a fixed buffer size for all message exchanges.

## Code
This project contains the following files:
- 1 server 
- 1 client
- 1 common file (.h and .cpp) used by both server/client containing a common Chat object that sends/receives messages, and can be reused by both the server and client.

## Build
To compile the server and client executables:
```
g++ -std=c++14 -pthread -o client client.cpp common.cpp
g++ -std=c++14 -pthread -o server server.cpp common.cpp
```
We need to compile against at least c++11 as we're using lambda functions; c++11 or c++14 both works. `-pthread` is needed because we're using std::thread.

## Running
To run:
```
./server
# in a separate terminal:
./client 0.0.0.0
```

You may now type your message into either the client or the server, and you should see the same message printed out in the other program. You can type /exit to exit the program. If you close the server connection, it will also result in the client program terminating.

## Assumptions

- The user will not send non-typeable characters like \x00 \x02 \x04 \x06 into the chat.
- We are sending 1 line at a time.
- Messages cannot be edited after being sent.
- Only 1 client connecting at a time. If multiple clients try to connect they will succeed in connecting but the server can only read messages from 1 client at a time, starting with the client that connected first, and each client must disconnect (with the user hitting "enter" at the server side) before the next client can be processed. Multiple clients connecting after the first client should not affect the first client's session.
