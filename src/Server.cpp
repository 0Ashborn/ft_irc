#include "Server.hpp"
#include "CommandHandler.hpp"
#include <fcntl.h>

Server::Server(int port, std::string& password): server_fd(-1), password(password) {
    FD_ZERO(&read_fds);
    this->port = port;
}

bool Server::init() {
    this->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->server_fd < 0) {
        std::cerr << "Socket creation failed: " << strerror(errno) << std::endl;
        return false;
    }
    std::cout << "Socket created successfully" << std::endl;
    fcntl(server_fd, F_SETFL, O_NONBLOCK);

    int opt = 1;
    setsockopt(this->server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(this->port);
    server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
    
    if (bind(this->server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "bind failed: " << strerror(errno) << std::endl;
        close(this->server_fd);
        return false;
    }
    std::cout << "Bind successfull on port " << this->port << std::endl;

    if (listen(this->server_fd, 10) < 0) {
        std::cerr << "Listen failed: " << strerror(errno) << std::endl;
        close(this->server_fd);
        return false;
    }

    std::cout << "Listening for connections..." << std::endl;
    return true;
}

void Server::run() {
    while (true) {
        FD_ZERO(&this->read_fds);
        FD_SET(this->server_fd, &this->read_fds);
        int max_fd = this->server_fd;

        for (size_t i = 0; i < users.size(); ++i) {
            int client_fd = users[i]->getFd();
            FD_SET(client_fd, &this->read_fds);
            if (client_fd > max_fd) {
                max_fd = client_fd;
            }
        }

        std::cout << "Waiting for activity...\n";
        int notification = select(max_fd+1, &this->read_fds, NULL, NULL, NULL);
        if(notification<0){
            perror("select error");
            break;
        }

        if (FD_ISSET(this->server_fd, &this->read_fds))
            acceptNewConnection();

        for (std::vector<User*>::iterator it = users.begin(); it != users.end(); ) {
            int client_fd = (*it)->getFd();

            if (FD_ISSET(client_fd ,&this->read_fds)) {
                if (!handleUserData(client_fd)) {
                    it = removeUser(it);
                } else {
                    ++it;
                }
            } else {
                ++it;
            }
        }
    }
}

User* Server::getUserByFd(int client_fd) {
    for (size_t i=0; i < users.size(); i++) {
        if(users[i]->getFd() == client_fd)
            return users[i];
    }
    return NULL;
}
int Server::handleUserData(int client_fd) {
    char buffer[1024];
    int bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes_read <= 0) {
        if (bytes_read == 0) {
            User* user = getUserByFd(client_fd);
            std::string serverResponse;
            for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it) {
                if ((*it)->hasUser(user)) {
                    if ((*it)->isChanOp(user)) {
                        (*it)->removeChanOp(user);
                    } else {
                        (*it)->removeRegularUser(user);
                    }
                    std::stringstream ss;
                    ss << user->getNickname() + "!" + user->getUsername() + "@" + "localhost PART " + (*it)->getChannelName();
                    ss << "\r\n";
                    serverResponse = ss.str();
                    std::vector<User*>::iterator itt;
                    for (itt = (*it)->getRegularUsers().begin(); itt != (*it)->getRegularUsers().end(); ++itt) {
                        sendToUser((*itt), serverResponse);
                    }
                    for (itt = (*it)->getChanOps().begin(); itt != (*it)->getChanOps().end(); ++itt) {
                        sendToUser((*itt), serverResponse);
                    }
                }
            }
            std::cout << "Client disconnected, socket fd: " << client_fd << "\n";
            std::cout << "----------------------------------------" << std::endl;
        } else {
            perror("recv failed");
        }
        return 0;
    } else {
        buffer[bytes_read] = '\0';
        User* user = getUserByFd(client_fd);
        if (user) {            
            std::vector<std::string> commands = user->handleIncomingData(buffer, bytes_read);
            
            for (size_t i = 0; i < commands.size(); i++) {
                std::string cmd = commands[i];
                std::string commandName;
                size_t spacePos = cmd.find(' ');
                if (spacePos != std::string::npos) {
                    commandName = cmd.substr(0, spacePos);
                } else {
                    commandName = cmd;
                }
                
                for (size_t j = 0; j < commandName.length(); j++) {
                    commandName[j] = toupper(commandName[j]);
                }
                
                CommandHandler::handleCommand(user, commands[i], *this);
                
                if (commandName == "QUIT") {
                    return 0;
                }
            }
        }
    }
    return 1;
}

void Server::acceptNewConnection() {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_sockfd = accept(this->server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_sockfd<0) {
        std::cerr << "Accept failed: " << strerror(errno) << std::endl;
        // close(this->server_fd); all existing client will get disconnected
        return ;
    } else {
        std::cout << "New connection, socket fd: " << client_sockfd << "\n";
        // client_fds.push_back(client_sockfd);

        fcntl(client_sockfd, F_SETFL, O_NONBLOCK);

        User* newUser = new User(client_sockfd);
        users.push_back(newUser);

        // const char* message = "client connected !\n";
        // send(client_sockfd, message, strlen(message), 0);
    }
    std::cout << "Connection accepted from " 
                << inet_ntoa(client_addr.sin_addr) << ":" 
                << ntohs(client_addr.sin_port) << std::endl;
}

std::vector<User*>::iterator Server::removeUser(std::vector<User*>::iterator it) {
    int client_fd = (*it)->getFd();
    close(client_fd);
    delete *it;
    // Update max_fd in case lmax howa li deleta (performance)
    //if uncommented need to add max_fd in the class member variable
    // if (client_fd == max_fd) {
    //     max_fd = server_fd;
    //     for (int fd : client_fds) {
    //         if (fd > max_fd) {
    //             max_fd = fd;
    //         }
    //     }
    // }
    return users.erase(it); 
}

void Server::removeUser(User* user) {
    std::vector<User*>::iterator it = std::find(users.begin(), users.end(), user);
    if (it != users.end())
        removeUser(it);
}

void Server::sendToUser(User* user, const std::string& serverResponse) {
    if (user && user->getFd() > 0) {
        send(user->getFd(), serverResponse.c_str(), serverResponse.length(), 0);
    }
}

void Server::cleanup() {
    for (std::vector<User*>::iterator it = users.begin(); it != users.end();) {
        close((*it)->getFd());
        delete *it;
        it = users.erase(it);
    }
    for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it) {
        removeChannel(*it);
    }
    if (server_fd != -1) {
        close(server_fd);
        server_fd = -1;
    }
    std::cout << "Connection closed" << std::endl;
}

std::string Server::getPassword() const {
    return password;
}

std::vector<Channel*>& Server::getChannels() {
    return channels;
}

void Server::addChannel(Channel* channel) {
    if (channel) {
        channels.push_back(channel);
    }
}

void Server::removeChannel(Channel* channel) {
    std::vector<Channel*>::iterator it = std::find(channels.begin(), channels.end(), channel);
    if (it != channels.end()) {
        delete (*it);
        channels.erase(it);
    }
}

Server::~Server() {
    cleanup();
}

std::vector<User*>& Server::getUsers() {
    return users;
}

User* Server::getUserByNickname(const std::string& nickname) {
    for (size_t i = 0; i < users.size(); ++i) {
        if (users[i]->getNickname() == nickname) {
            return users[i];
        }
    }
    return NULL;
}

// void Server::sendToUser(User* user, const std::string& message) {
//     if (user && user->getFd() > 0) {
//         send(user->getFd(), message.c_str(), message.length(), 0);
//     }
// }
