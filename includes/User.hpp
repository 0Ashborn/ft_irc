#pragma once

#include <string>
#include <iostream>
#include <vector>

class User {
    private:
        int fd;
        std::string nickname;
        std::string username;
        std::string realname;
        std::string messageBuffer;
        bool authenticated;
        bool givenPassword;
        bool givenNickname;
        bool givenUsername;
        std::vector<std::string> channels;
    
    public:
        User(int fd);
        ~User();

        // Getters
        int getFd();
        std::string& getNickname();
        std::string& getUsername();

        // Booleans
        bool hasGivenPassword() const;
        bool hasGivenNickname() const;
        bool hasGivenUsername() const;
        bool isAuthenticated();

        // Setters
        void setNickname(std::string& nickname);
        void setUsername(std::string& username);
        void setRealname(std::string& realname);
        void setAuthenticated(bool status);
        void setGivenPassword(bool status);
        void setGivenNickname(bool status);
        void setGivenUsername(bool status);

        bool isNickInUse(const std::string& nickname, const std::vector<User*> users);

        std::vector<std::string> handleIncomingData(char* data, int length);
        // std::vector<std::string> extractCommands();
};
