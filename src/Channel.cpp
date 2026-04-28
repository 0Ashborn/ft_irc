#include "Channel.hpp"

Channel::Channel(std::string chName, std::string password, std::string topic)
    : chName(chName), password(password), topic(topic), 
      inviteOnly(false), privateCh(false), topicProtected(true), userLimit(0) {}

// Searching for users
Channel* Channel::findChannel(const std::vector<Channel*> channels, const std::string& channel) {
    std::vector<Channel*>::const_iterator it;
    
    for (it = channels.begin(); it != channels.end(); ++it) {
        if ((*it)->chName == channel) {
            return (*it);
        }
    }
    
    return NULL;
}

User* Channel::findRegularUser(User* user) {
    std::vector<User*>::iterator it = std::find(regularUsers.begin(), regularUsers.end(), user);
    if (it != regularUsers.end()) {
        return user;
    }
    return NULL;
}

User* Channel::findChanop(User* user) {
    std::vector<User*>::iterator it = std::find(chanOps.begin(), chanOps.end(), user);
    if (it != chanOps.end()) {
        return user;
    }
    return NULL;
}

// Booleans
bool Channel::isChanOp(User* user) {
    std::vector<User*>::iterator it = std::find(chanOps.begin(), chanOps.end(), user);
    if (it != chanOps.end())
        return true;
    return false;
}

bool Channel::isValidChannelName(std::string channelName) {
    if (channelName.length() <= 1 || (channelName[0] != '#' && channelName[0] != '&')) {
        return false;
    }
    if (channelName.length() > 50) {
        return false;
    }
    
    std::string allowedChars = "#_-[]{}|^";
    for (size_t i = 1; i < channelName.length(); i++) {
        if (!isalnum(channelName[i]) && allowedChars.find(channelName[i]) == std::string::npos) {
            return false;
        }
    }

    return true;
}

bool Channel::isInviteOnly() const {
    return inviteOnly;
}

bool Channel::isPrivateCh() const {
    return privateCh;
}

bool Channel::isUserInvited(User* user) {
    std::vector<User*>::iterator it = std::find(invitedUsers.begin(), invitedUsers.end(), user);
    if (it != invitedUsers.end()) {
        return true;
    }
    return false;
}

bool Channel::hasUser(User* user) {
    if (findRegularUser(user) || findChanop(user)) {
        return true;
    }
    return  false;
}

// Getters
std::vector<User*>& Channel::getRegularUsers() {
    return regularUsers;
}

std::vector<User*>& Channel::getChanOps() {
    return chanOps;
}

std::vector<User*>& Channel::getInvitedUsers() {
    return invitedUsers;
}

std::string Channel::getPassword() const {
    return password;
}

std::string Channel::getChannelName() const {
    return chName;
}

// Channel utils
void Channel::addChanOp(User* user) {
    if (!findChanop(user)) {
        chanOps.push_back(user);
    }
}

void Channel::addRegularUser(User* user) {
    if (!findRegularUser(user)) {
        regularUsers.push_back(user);
    }
}

void Channel::removeRegularUser(User* user) {
    if (findRegularUser(user)) {
        std::vector<User*>::iterator it = std::find(regularUsers.begin(), regularUsers.end(), user);
        regularUsers.erase(it);
    }
}

void Channel::removeChanOp(User* user) {
    if (findChanop(user)) {
        std::vector<User*>::iterator it = std::find(chanOps.begin(), chanOps.end(), user);
        chanOps.erase(it);
    }
}

void Channel::grantOpPrivilege(User* user) {
    if (findRegularUser(user)) {
        removeRegularUser(user);
        addChanOp(user);
    }
}

void Channel::revokeOpPrivilege(User* user) {
    if (findChanop(user)) {
        removeChanOp(user);
        addRegularUser(user);        
    }
}

// Server related methods
void Channel::sendJoinMessage(Server& server, User* joinedUser) {
    std::stringstream ss;
    std::string serverResponse;

    ss << ":" << joinedUser->getNickname() << "!" << joinedUser->getUsername()
       << "@localhost JOIN :" << chName << "\r\n";
    serverResponse = ss.str();
    for (std::vector<User*>::iterator it = chanOps.begin(); it != chanOps.end(); ++it) {
        server.sendToUser(*it, serverResponse);
    }
    for (std::vector<User*>::iterator it = regularUsers.begin(); it != regularUsers.end(); ++it) {
        server.sendToUser(*it, serverResponse);
    }
}

void Channel::sendTopic(Server& server, User* joinedUser) {
    std::stringstream ss;

    if (topic.empty()) {
        ss << ":localhost " << 331 << " " + joinedUser->getNickname() << " " + chName << " :No topic is set\r\n";
        return server.sendToUser(joinedUser, ss.str());
    }
    ss << ":localhost " << 332 << " " + joinedUser->getNickname() << " " + chName << " :" + topic << "\r\n";
    server.sendToUser(joinedUser, ss.str());
}

void Channel::sendChannelUsers(Server& server, User* joinedUser) {
    std::stringstream ss;
    std::stringstream allUsers;

    for (std::vector<User*>::iterator it = chanOps.begin(); it != chanOps.end(); ++it) {
        allUsers << "@" << (*it)->getNickname() << " ";
    }

    for (std::vector<User*>::iterator it = regularUsers.begin(); it != regularUsers.end(); ++it) {
        allUsers << (*it)->getNickname() << " ";
    }

    ss << ":localhost " << 353 << " " + joinedUser->getNickname() << " = " + chName << " :" << allUsers.str() << "\r\n";
    ss << ":localhost " << 366 << " " + joinedUser->getNickname() << " " << chName << " :End of /NAMES list\r\n";

    server.sendToUser(joinedUser, ss.str());
}

void Channel::broadcastJoinMessage(Server& server, User* joinedUser) {
    this->sendJoinMessage(server, joinedUser);
    this->sendTopic(server, joinedUser);
    this->sendChannelUsers(server, joinedUser);
}

//===========SALAH PART===================================================

bool Channel::isTopicProtected() const {
    return topicProtected;
}

int Channel::getUserLimit() const {
    return userLimit;
}

void Channel::setPassword(const std::string& newPassword) {
    password = newPassword;
}

void Channel::setInviteOnly(bool invite) {
    inviteOnly = invite;
}

void Channel::setPrivateCh(bool priv) {
    privateCh = priv;
}

void Channel::setTopicProtected(bool protect) {
    topicProtected = protect;
}

void Channel::setUserLimit(int limit) {
    userLimit = limit;
}

void Channel::setTopic(const std::string& newTopic) {
    topic = newTopic;
}

std::string Channel::getTopic() const {
    return topic;
}
