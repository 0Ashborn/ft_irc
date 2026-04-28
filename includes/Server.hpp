#pragma once

#include <iostream>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <algorithm>
#include <cstdio>
#include <vector>
#include <cstdlib>
#include "User.hpp"
#include "Channel.hpp"

class Channel;

class Server{
    private:
        int server_fd;
        int port;
        std::string password;
        std::vector<User*> users;
        fd_set read_fds;
        std::vector<Channel*> channels;

        void acceptNewConnection();
        int handleUserData(int client_fd);
        std::vector<User*>::iterator removeUser(std::vector<User*>::iterator it);
        void cleanup();
        
    public:
        Server(int port, std::string& password);
        bool init();
        void run();
        
        User* getUserByFd(int client_fd);
        std::string getPassword() const;
        std::vector<User*>& getUsers();
        std::vector<Channel*>& getChannels();

        void addChannel(Channel* channel);
        void removeChannel(Channel* channel);
        void removeUser(User* user);
        void sendToUser(User* user, const std::string& serverResponse);
        User* getUserByNickname(const std::string& nickname);
        // void sendToUser(User* user, const std::string& message);
        ~Server();
};
