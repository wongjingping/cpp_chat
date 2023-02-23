#include "common.h"

int start_as_server()
{
    int server_fd, client_fd;
    sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        std::cerr << "Failed to create socket.\n";
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (sockaddr *)&address, sizeof(address)) < 0)
    {
        std::cerr << "Failed to bind to port " << PORT << ".\n";
        return 1;
    }

    if (listen(server_fd, 1) < 0)
    {
        std::cerr << "Failed to listen on socket.\n";
        return 1;
    }

    std::cout << "Server is listening on port " << PORT << "...\n";

    while (true)
    {
        std::cout << "Waiting for connections\n";
        if ((client_fd = accept(server_fd, (sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            std::cerr << "Failed to accept connection.\n";
            continue;
        }

        std::cout << "Connected to client, start chatting!" << std::endl << \
        "Type /exit to close the app" << std::endl;

        Chat chat(client_fd, true);

        while (chat.connected()) {
            // reduce connection checking frequency
            std::this_thread::sleep_for(sleep_time);
        }
        close(client_fd);
        // end if user indicated to exit, else wait for next connection
        if (chat.user_exit()) { 
            close(server_fd);
            return 0; 
        }
    }

    return 0;
}

int main()
{
    return start_as_server();
}
