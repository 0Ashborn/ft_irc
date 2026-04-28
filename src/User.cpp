#include "User.hpp"

User::User(int fd): fd(fd), nickname(""),
    username(""), authenticated(false), givenPassword(false), 
    givenNickname(false), givenUsername(false) {}

// Getters
int User::getFd() {
    return fd;
}
std::string& User::getNickname() {
    return nickname;
}

std::string& User::getUsername() {
    return username;
}

bool User::isAuthenticated() {
    return authenticated;
}

bool User::hasGivenNickname() const {
    return givenNickname;
}

bool User::hasGivenPassword() const {
    return givenPassword;
}

bool User::hasGivenUsername() const {
    return givenUsername;
}

// Setters
void User::setGivenNickname(bool status) {
    givenNickname = status;
}

void User::setGivenPassword(bool status) {
    givenPassword = status;
}

void User::setGivenUsername(bool status) {
    givenUsername = status;
}

void User::setNickname(std::string& nickname) {
    this->nickname = nickname;
}

void User::setUsername(std::string& username) {
    this->username = username;
}

void User::setAuthenticated(bool status) {
    authenticated = status;
}

void User::setRealname(std::string& realname) {
    this->realname = realname;
}

//problem was here i fix it \r
std::vector<std::string> User::handleIncomingData(char* data, int length) {
    messageBuffer.append(data, length);

    std::vector<std::string> commands;
    size_t pos = 0;

    while ((pos = messageBuffer.find('\n')) != std::string::npos) {
        std::string cmd = messageBuffer.substr(0, pos);
        
        if (!cmd.empty() && cmd[cmd.length() - 1] == '\r') {
            cmd.erase(cmd.length() - 1);
        }
        
        commands.push_back(cmd);
        messageBuffer.erase(0, pos + 1);
    }

    // pos = 0;
    // while((pos = messageBuffer.find("\n")) != std::string::npos){
    //     std::string cmd = messageBuffer.substr(0, pos);
    //     commands.push_back(cmd);
    //     messageBuffer.erase(0, pos+1);
    // }

    return commands;
}

bool User::isNickInUse(const std::string& nickname, const std::vector<User*> users) {
    std::vector<User*>::const_iterator it;
    for (it = users.begin(); it != users.end(); ++it) {
        if (nickname == (*it)->getNickname()) {
            return true;
        }
    }
    return false;
}

User::~User() {}
