#include "Server.hpp"

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
    }
    if (std::string(argv[1]).empty()) {
        std::cerr << "Invalid port!" << std::endl;
        return 1;
    }
    int i = 0;
    while (argv[1][i]) {
        if (!isdigit(argv[1][i])) {
            std::cerr << "Invalid port!" << std::endl;
            return 1;
        }
        i++;
    }
    int port = atoi(argv[1]);
    if (port > 65535 || port < 0) {
        std::cerr << "Invalid port!" << std::endl;
        return 1;
    }
    std::string password = argv[2];
    if (password.empty()) {
        std::cerr << "Invalid password!" << std::endl;
        return 1;
    }

    Server IRCserver(port, password);
    if (!IRCserver.init()) {
        std::cerr << "Failed to start server" << std::endl;
        return 1;
    }
    
    std::cout << "Starting server loop..." << std::endl;
    IRCserver.run();
    return 0;
}
