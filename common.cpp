#include "common.h"

// we use standard ascii byte codes for start, end, ack
// https://www.asciitable.com/
const char *START_MSG = "\x02";
const char *END_MSG = "\x03";
const char *ACK_MSG = "\x06";

/**
 * Kick off the thread receiving messages, and another that waits and sends
 * messages via the socket.
 */
Chat::Chat(int sock_fd, bool is_server) : sock_fd(sock_fd)
{
    this->exit = false;
    if (is_server)
    {
        server_connected = true;
    }
    else
    {
        client_connected = true;
    }
    this->send_buf[0] = '\x00';
    this->rcv_buf[0] = '\x00';
    std::thread receiver(std::bind(&Chat::receive_msg, this));
    receiver.detach();
    std::thread sender(std::bind(&Chat::wait_and_send_msg, this));
    sender.detach();
    std::cout << "Ready to send and receive messages" << std::endl;
}

/**
 * sends a message and waits for an ack.
 * prints out the roundtrip time if required.
 */
void Chat::send_msg(const char *msg, size_t len, bool verbose)
{
    start_time = std::chrono::high_resolution_clock::now();
    // Send the message in chunks of fixed size
    int total_sent = 0;
    int sent = 0;
    int start_bytes = ::send(sock_fd, START_MSG, MSG_LEN, 0);
    if (start_bytes == -1)
    {
        std::cerr << "Failed to send START_MSG." << std::endl;
        return;
    }
    while (total_sent < len)
    {
        int remaining = len - total_sent;
        sent = ::send(sock_fd, msg + total_sent, BUF_SIZE, 0);
        if (sent == -1)
        {
            std::cerr << "Failed to send message:" << msg + total_sent << std::endl;
            break;
        }
        total_sent += sent;
    }
    int end_bytes = ::send(sock_fd, END_MSG, MSG_LEN, 0);
    if (end_bytes == -1)
    {
        std::cerr << "Failed to send END_MSG." << std::endl;
        return;
    }
}

/**
 * waits for msg on stdin and sends with send_msg.
 * this loops forever until `/exit` is received.
 */
void Chat::wait_and_send_msg()
{
    std::string msg;
    while (true)
    {
        std::getline(std::cin, msg);
        if (msg == "/exit")
        {
            std::cout << "Closing connection...\n";
            close(sock_fd);
            server_connected = false;
            client_connected = false;
            exit = true;
            return;
        }
        send_msg(msg.c_str(), msg.length(), true);
    }
}

/**
 * waits for the next message, and sends back an ack upon receipt
 */
void Chat::receive_msg()
{
    while (true)
    {
        std::memset(rcv_buf, 0, BUF_SIZE);
        int num_bytes = ::recv(sock_fd, rcv_buf, BUF_SIZE, 0);
        if (num_bytes == -1)
        {
            return; // when connection closes
        }
        if (num_bytes == 0)
        {
            if (server_connected)
            {
                std::cout << "Client disconnected. Please wait for a new client,"
                          << " or type /exit to end this session." << std::endl;
                server_connected = false;
            }
            if (client_connected)
            {
                std::cout << "Server disconnected." << std::endl;
                client_connected = false;
            }

            return;
        }
        rcv_buf[num_bytes] = '\0';

        /**
         * We have 4 normal cases for receiving a message back:
         * receiver receives START_MSG from sender
         * receiver receives END_MSG from sender
         * sender receives ACK_MSG from receiver
         * receiver receives normal string body from sender
         */
        if (std::memcmp(START_MSG, rcv_buf, 1) == 0)
        { // rcv gets start of msg and prints msg header
            std::cout << "--start of message--" << std::endl;
        }
        else if (std::memcmp(END_MSG, rcv_buf, 1) == 0)
        { // rcv sends ack when end reached
            std::cout << "\n--end of message--" << std::endl;
            int nbytes = ::send(sock_fd, ACK_MSG, MSG_LEN, 0);
        }
        else if (std::memcmp(ACK_MSG, rcv_buf, 1) == 0)
        {
            // print round trip
            time_point_t end_time = std::chrono::high_resolution_clock::now();
            std::cout << "Roundtrip time: "
                      << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count()
                      << " microseconds" << std::endl;
        }
        else
        { // rcv prints msg body
            std::cout << rcv_buf << std::flush;
        }
    }
}

bool Chat::connected()
{
    return server_connected || client_connected;
}

bool Chat::user_exit()
{
    return exit;
}