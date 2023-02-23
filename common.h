#ifndef CHAT_COMMON_H
#define CHAT_COMMON_H

#include <arpa/inet.h>
#include <chrono>
#include <cstring>
#include <functional>
#include <iostream>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

typedef std::chrono::high_resolution_clock::time_point time_point_t;

// common variables used by both server/client
const int PORT = 8888;
const int BUF_SIZE = 16;
const int MSG_LEN = BUF_SIZE; // length used by special messages
extern const char* START_MSG;
extern const char* END_MSG;
extern const char* ACK_MSG;
const std::chrono::milliseconds sleep_time(100); // ms between connection checks

/**
 * This class holds on to the socket, maintains the session state, and allows one
 * to send/receive messages. We reuse it in the server and client main executables.
*/
class Chat
{
public:
    Chat(int sock_fd, bool is_server);

    /**
     * sends a message and waits for an ack.
     * prints out the roundtrip time if required.
     */
    void send_msg(const char *msg, size_t len, bool verbose);

    /**
     * waits for msg on stdin and sends with send_msg.
     * this loops forever until `/exit` is received.
    */
    void wait_and_send_msg();

    /**
     * waits for the next message, and sends back an ack upon receipt
     */
    void receive_msg();

    // true if still connected
    bool connected();

    // true if user indicated to exit
    bool user_exit();

private:
    bool server_connected;
    bool client_connected;
    bool exit;
    int sock_fd;
    char send_buf[BUF_SIZE];
    char rcv_buf[BUF_SIZE];
    std::string name;
    time_point_t start_time;
};

#endif
