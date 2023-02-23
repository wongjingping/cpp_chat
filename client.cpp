#include "common.h"

int start_as_client(const char *server_ip)
{
    int sock_fd;
    sockaddr_in server_address;

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "Failed to create socket.\n";
        return 1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, server_ip, &server_address.sin_addr) <= 0)
    {
        std::cerr << "Invalid server IP address.\n";
        return 1;
    }

    if (connect(sock_fd, (sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        std::cerr << "Failed to connect to server.\n";
        return 1;
    }

    std::cout << "Connected to server, start chatting!\n"
              << "Type /exit to close the app\n";

    Chat chat(sock_fd, false);
    while (chat.connected()) {
        // reduce connection checking frequency
        std::this_thread::sleep_for(sleep_time);
    }
    close(sock_fd);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <server-ip>\n";
        return 1;
    }
    return start_as_client(argv[1]);
}
