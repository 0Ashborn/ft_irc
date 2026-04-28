#include "CommandHandler.hpp"

std::vector<std::string> CommandHandler::parseCommand(const std::string& cmd){
    std::string prefix;
    std::string trailing;
    std::string rest = cmd;
    std::vector<std::string> tokens;
    size_t space_pos ;

    if (!cmd.empty() && cmd[0] == ':') {
        space_pos = cmd.find(' ');
        if (space_pos != std::string::npos) {
            prefix = cmd.substr(1, space_pos - 1);
            rest = cmd.substr(space_pos + 1);
            // tokens.push_back(prefix);
        } else {
            return tokens;
        }
    }

    space_pos = 0;

    space_pos = rest.find(" :");
    if (space_pos != std::string::npos) {
        trailing = rest.substr(space_pos + 1);
        rest = rest.substr(0, space_pos);
    } 

    space_pos = 0;
    size_t i = 0;

    space_pos = rest.find(' ');
    while (space_pos != std::string::npos) {
        std::string token = rest.substr(i, space_pos - i);
        if (!token.empty()) {
            tokens.push_back(token);
        }
        i = space_pos+1;
        
        while (i < rest.length() && rest[i] == ' ') {
            i++;
        }
        space_pos = rest.find(' ', i);
    }
    if (i < rest.length()) {
        std::string token = rest.substr(i);
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }

    if (!trailing.empty()) {
        tokens.push_back(trailing);
    }
    
    // For debugging purposes
    // std::cout << "[";
    // for(std::vector<std::string>::iterator it = tokens.begin(); it != tokens.end(); ++it) {
    //     std::cout << "'" << *it << "'";
    //     if (it + 1 != tokens.end()) {
    //         std::cout << ", ";
    //     }
    // }
    // std::cout << "]" << std::endl;

    return tokens;
}

std::string normalizeCommand(const std::string& command) {
    std::string newCommand;
    for (size_t i = 0; i < command.size(); i++) {
        newCommand += toupper(command[i]);
    }
    return newCommand;
}

void CommandHandler::handleCommand(User* user, const std::string& cmd, Server& server){
    std::vector<std::string> tokens = parseCommand(cmd);
    std::string command;
    if (!tokens.empty()) {
        command = normalizeCommand(tokens[0]);
    }

    if (command == "PASS") {
        handlePASS(user, tokens, server);
    } else if (command == "USER") {
        handleUSER(user, tokens, server);
    } else if (command == "NICK") {
        handleNICK(user, tokens, server);
    } else if (command == "QUIT") {
        handleQUIT(user, tokens, server);
    } else if (command == "JOIN") {
        handleJOIN(user, tokens, server);
    } else if (command == "PART") {
        handlePART(user, tokens, server);
    } else if (command == "PING") {
        handlePING(user, tokens, server);
    } else if (command == "PRIVMSG") {
        handlePRIVMSG(user, tokens, server);
    } else if (command == "KICK") {
        handleKICK(user, tokens, server);
    } else if (command == "INVITE") {
        handleINVITE(user, tokens, server);
    } else if (command == "TOPIC") {
        handleTOPIC(user, tokens, server);
    } else if (command == "MODE") {
        handleMODE(user, tokens, server);
    } else {
        handleUKNOWN(user, tokens, server);
    }
}

std::string CommandHandler::generateResponse(const std::string& serverName, int statusCode, 
        const std::string& target, const std::string& message) {
    std::stringstream ss;
    std::string recv = target.empty() ? "*" : target;

    ss << ":" << serverName << " "
       << statusCode << " "
       << recv << " "
       << ":" << message << "\r\n";

    return ss.str();
}

// Authentication Commands
void CommandHandler::handlePASS(User* user, const std::vector<std::string>& params, Server& server) {
    std::string serverResponse;
    
    if (user->hasGivenPassword() || user->isAuthenticated()) {
        serverResponse = generateResponse("localhost", 462, user->getNickname(), "You may not reregister");
        return server.sendToUser(user, serverResponse);   
    }
    if (params.size() == 1) {
        serverResponse = generateResponse("localhost", 461, user->getNickname(), "PASS: Not enough params");
        return server.sendToUser(user, serverResponse);
    }
    if (params[1] != server.getPassword()) {
        serverResponse = generateResponse("localhost", 464, user->getNickname(), "Password incorrect");
        return server.sendToUser(user, serverResponse);
    }
    user->setGivenPassword(true);
}

void CommandHandler::handleNICK(User* user, const std::vector<std::string>& params, Server& server) {
    std::string serverResponse;
    std::string allowedChars = "[]{}_-\\|`^";

    if (!user->hasGivenPassword()) {
        serverResponse = generateResponse("localhost", 452, user->getNickname(), "You have not registred");
        return server.sendToUser(user, serverResponse);
    }
    if (params.empty()) {
        serverResponse = generateResponse("localhost", 431, user->getNickname(), "Missing params");
        return server.sendToUser(user, serverResponse);
    }
    std::string checkNickname = params[1];
    if (user->isNickInUse(checkNickname, server.getUsers())) {
        serverResponse = generateResponse("localhost", 433, user->getNickname(), "Nickname is already in use");
        return server.sendToUser(user, serverResponse);
    }
    if (checkNickname.size() > 9 || (!isalpha(checkNickname[0]) && allowedChars.find(checkNickname[0]) == std::string::npos)) {
        serverResponse = generateResponse("localhost", 432, user->getNickname(), "Erroneous nickname");
        return server.sendToUser(user, serverResponse);
    }
    for (size_t i = 1; i < checkNickname.size(); i++) {
        if (!isalnum(checkNickname[i]) && allowedChars.find(checkNickname[i]) == std::string::npos) {
            serverResponse = generateResponse("localhost", 432, user->getNickname(), "Erroneous nickname");
            return server.sendToUser(user, serverResponse);
        }
    }
    user->setGivenNickname(true);
    std::string newNickname = checkNickname;
    user->setNickname(newNickname);
}

void CommandHandler::handleUSER(User* user, const std::vector<std::string>& params, Server& server) {
    std::string serverResponse;
    std::string allowedChars = "-_.~";

    if (!user->hasGivenPassword() || !user->hasGivenNickname()) {
        serverResponse = generateResponse("localhost", 452, user->getNickname(), "You have not registred");
        return server.sendToUser(user, serverResponse);
    }
    if (user->hasGivenUsername()) {
        serverResponse = generateResponse("localhost", 462, user->getNickname(), "Already registred");
        return server.sendToUser(user, serverResponse);
    }
    if (params.size() < 5) {
        serverResponse = generateResponse("localhost", 461, user->getNickname(), "Need more params");
        return server.sendToUser(user, serverResponse);
    }

    std::string username = params[1];
    for (size_t i = 0; i < username.length(); i++) {
        if (!isalnum(username[i]) && allowedChars.find(username[i]) == std::string::npos) {
            serverResponse = generateResponse("localhost", 432, user->getNickname(), "error username");
            return server.sendToUser(user, serverResponse);
        }
    }

    std::string realname = params[4];

    if (realname.empty() || realname[0] != ':') {
        serverResponse = generateResponse("localhost", 462, user->getNickname(), "error empty param");
        return server.sendToUser(user, serverResponse);
    }
    if (realname[0] == ':') {
        realname = realname.substr(1);
    }
    user->setUsername(username);
    user->setRealname(realname);
    user->setGivenUsername(true);
    user->setAuthenticated(true);
    
    // Sending welcome message here 
    std::string response = generateResponse("localhost", 001, user->getNickname(), "almandobia tora7ibo bikom");
    response += generateResponse("localhost", 002, user->getNickname(), "Your host is localhost");
    response += generateResponse("localhost", 003, user->getNickname(), "This server was created on Oct 2025");
    response += generateResponse("localhost", 004, user->getNickname() + " localhost", "1.0 ao mtov");
    
    server.sendToUser(user, response);
}

// User related commands
void CommandHandler::handleQUIT(User* user, const std::vector<std::string>& params, Server& server) {
    std::string serverResponse;

    if (!user->isAuthenticated()) {
        serverResponse = generateResponse("localhost", 451, user->getNickname(), "You have not registred");
        return server.sendToUser(user, serverResponse);
    }
    // Send a message to the client
    if (params.size() == 1) {
        server.sendToUser(user, "Client quit\r\n");
    }
}

void CommandHandler::handleJOIN(User* user, const std::vector<std::string>& params, Server& server) {
    std::string serverResponse;

    // Basic validation
    if (!user->isAuthenticated()) {
        serverResponse = generateResponse("localhost", 452, user->getNickname(), "You have not registred");
        return server.sendToUser(user, serverResponse);
    }
    if (params.size() == 1) {
        serverResponse = generateResponse("localhost", 461, user->getNickname(), "Need more params");
        return server.sendToUser(user, serverResponse);        
    }

    std::string channelName = params[1];
    if (!Channel::isValidChannelName(channelName)) {
        serverResponse = generateResponse("localhost", 403, user->getNickname(), "Invalid channel name");
        return server.sendToUser(user, serverResponse);        
    }

    std::vector<Channel*>& channels = server.getChannels();

    // We create the channel if it does not exist and add the user as a chanop
    Channel* targetChannel = Channel::findChannel(channels, channelName);
    if (targetChannel == NULL) {
        Channel* newChannel = new Channel(channelName);
        newChannel->addChanOp(user);
        server.addChannel(newChannel);
        newChannel->broadcastJoinMessage(server, user);
    } else {
        if (targetChannel->hasUser(user)) {
            return;
        }
        int userCount = targetChannel->getRegularUsers().size();
        userCount += targetChannel->getChanOps().size();
        if (targetChannel->getUserLimit() > 0 && userCount + 1 > targetChannel->getUserLimit()) {
            serverResponse = generateResponse("localhost", 473, user->getNickname(), "Maximum user limit reached");
            return server.sendToUser(user, serverResponse);
        }
        // Checking if the channel is on invite only mode
        if (targetChannel->isInviteOnly() && !targetChannel->isUserInvited(user)) {
            serverResponse = generateResponse("localhost", 473, user->getNickname(), "You must have an invitation in order to join this channel");
            return server.sendToUser(user, serverResponse);
        }
        // Checking if the channel has a password
        if (targetChannel->isPrivateCh()) {
            if (params.size() < 3) {
                serverResponse = generateResponse("localhost", 475, user->getNickname(), "You must provide a password to join this channel");
                return server.sendToUser(user, serverResponse);
            }
            std::string password = params[2];
            if (password != targetChannel->getPassword()) {
                serverResponse = generateResponse("localhost", 475, user->getNickname(), "Wrong channel password");
                return server.sendToUser(user, serverResponse);
            }
        }
        // Broadcasting the join message and adding the user to the channel
        targetChannel->addRegularUser(user);
        targetChannel->broadcastJoinMessage(server, user);
    }
}

void CommandHandler::handlePART(User* user, const std::vector<std::string>& params, Server& server) {
    std::string serverResponse;
    
    // Basic checks
    if (!user->isAuthenticated()) {
        serverResponse = generateResponse("localhost", 451, user->getNickname(), "You have not registered");
        return server.sendToUser(user, serverResponse);
    }
    if (params.size() < 2) {
        serverResponse = generateResponse("localhost", 461, user->getNickname(), "Need more params");
        return server.sendToUser(user, serverResponse);
    }
    // Checking if the channel already exists
    std::vector<Channel*>& channels = server.getChannels();
    std::string channelName = params[1];
    
    Channel* channel = Channel::findChannel(channels, channelName);
    if (channel == NULL) {
        serverResponse = generateResponse("localhost", 403, user->getNickname(), "Channel does not exist");
        return server.sendToUser(user, serverResponse);
    }
    if (!channel->hasUser(user)) {
        serverResponse = generateResponse("localhost", 442, user->getNickname(), "You are not a member of this channel");
        return server.sendToUser(user, serverResponse);        
    }
    // Message to broadcast to all members
    std::stringstream ss;
    ss << ":" + user->getNickname() + "!" + user->getUsername() + "@" + "localhost PART " + channelName;
    if (params.size() == 3) {
        ss << params[2];
    }
    ss << "\r\n";
    serverResponse = ss.str();
    std::vector<User*>::iterator it;
    for (it = channel->getRegularUsers().begin(); it != channel->getRegularUsers().end(); ++it) {
        server.sendToUser((*it), serverResponse);
    }
    for (it = channel->getChanOps().begin(); it != channel->getChanOps().end(); ++it) {
        server.sendToUser((*it), serverResponse);
    }

    // Remove the user from the channel
    if (channel->isChanOp(user)) {
        channel->removeChanOp(user);
    } else {
        channel->removeRegularUser(user);
    }
    if (channel->getRegularUsers().empty() && channel->getChanOps().empty()) {
        server.removeChannel(channel);
    }
}

void CommandHandler::handlePING(User* user, const std::vector<std::string>& params, Server& server) {
    if (params.size() < 2) {
        return;
    }
    std::string token = params[1];
    std::string serverResponse = "PONG " + token + "\r\n";
    server.sendToUser(user, serverResponse);
}

//SALAH PART OF IRC commands.

// private msg code

void CommandHandler::handlePRIVMSG(User* user, const std::vector<std::string>& params, Server& server) {
    std::string serverResponse;

    // Basic validation
    if (!user->isAuthenticated()) {
        serverResponse = generateResponse("localhost", 451, user->getNickname(), "You have not registered");
        return server.sendToUser(user, serverResponse);
    }
    
    if (params.size() < 3) {
        serverResponse = generateResponse("localhost", 461, user->getNickname(), "PRIVMSG :Not enough parameters");
        return server.sendToUser(user, serverResponse);
    }

    std::string target = params[1];
    std::string message = params[2];

    if (message.empty() || message[0] != ':') {
        serverResponse = generateResponse("localhost", 412, user->getNickname(), "PRIVMSG :Syntax error - message must start with colon");
        return server.sendToUser(user, serverResponse);
    }
    if (message[0] == ':') {
        message = message.substr(1);
    }

    if (message.empty()) {
        serverResponse = generateResponse("localhost", 412, user->getNickname(), "No text to send");
        return server.sendToUser(user, serverResponse);
    }

    // Check if target is a channel (starts with # or &)
    if (target[0] == '#' || target[0] == '&') {
        handleChannelPRIVMSG(user, target, message, server);
    } else {
        handlePrivatePRIVMSG(user, target, message, server);
    }
}

void CommandHandler::handleChannelPRIVMSG(User* user, const std::string& channelName, 
                                         const std::string& message, Server& server) {
    std::string serverResponse;

    // Find the channel
    Channel* channel = Channel::findChannel(server.getChannels(), channelName);
    if (!channel) {
        serverResponse = generateResponse("localhost", 403, user->getNickname(), channelName + " :No such channel");
        return server.sendToUser(user, serverResponse);
    }

    // Check if user is in the channel
    if (!channel->hasUser(user)) {
        serverResponse = generateResponse("localhost", 404, user->getNickname(), channelName + " :Cannot send to channel");
        return server.sendToUser(user, serverResponse);
    }

    // Create the PRIVMSG format
    std::string privmsg = ":" + user->getNickname() + "!" + user->getUsername() + 
                         "@localhost PRIVMSG " + channelName + " :" + message + "\r\n";

    // Broadcast to all channel members except the sender
    broadcastToChannelExceptSender(channel, privmsg, user, server);
}

void CommandHandler::handlePrivatePRIVMSG(User* user, const std::string& targetNick, const std::string& message, Server& server)
{
    std::string serverResponse;

    User* targetUser = server.getUserByNickname(targetNick);
    if (!targetUser) {
        serverResponse = generateResponse("localhost", 401, user->getNickname(), targetNick + " :No such nick/channel");
        return server.sendToUser(user, serverResponse);
    }

    // Don't allow sending to yourself
    if (targetUser == user) {
        return;
    }

    // Create the PRIVMSG format
    std::string privmsg = ":" + user->getNickname() + "!" + user->getUsername() + 
                         "@localhost PRIVMSG " + targetNick + " :" + message + "\r\n";

    server.sendToUser(targetUser, privmsg);
}

// ==================== TOPIC COMMAND ====================
void CommandHandler::handleTOPIC(User* user, const std::vector<std::string>& params, Server& server) {
    std::string serverResponse;

    if (!user->isAuthenticated()) {
        serverResponse = generateResponse("localhost", 451, user->getNickname(), "You have not registered");
        return server.sendToUser(user, serverResponse);
    }
    
    if (params.size() < 2) {
        serverResponse = generateResponse("localhost", 461, user->getNickname(), "TOPIC :Not enough parameters");
        return server.sendToUser(user, serverResponse);
    }

    std::string channelName = params[1];

    Channel* channel = Channel::findChannel(server.getChannels(), channelName);
    if (!channel) {
        serverResponse = generateResponse("localhost", 403, user->getNickname(), channelName + " :No such channel");
        return server.sendToUser(user, serverResponse);
    }

    if (!channel->hasUser(user)) {
        serverResponse = generateResponse("localhost", 442, user->getNickname(), channelName + " :You're not on that channel");
        return server.sendToUser(user, serverResponse);
    }

    if (params.size() == 2) {
        displayTopic(user, channel, server);
    } else {
        std::string newTopic = params[2];
        
        if (newTopic.empty() || newTopic[0] != ':') {
            serverResponse = generateResponse("localhost", 461, user->getNickname(), "TOPIC :Syntax error - topic must start with colon");
            return server.sendToUser(user, serverResponse);
        }
        if (newTopic[0] == ':') {
            newTopic = newTopic.substr(1);
        }
        
        setTopic(user, channel, newTopic, server);
    }
}

void CommandHandler::displayTopic(User* user, Channel* channel, Server& server)
{
    std::string topic = channel->getTopic();
    
    if (topic.empty()) {
        std::string response = generateResponse("localhost", 331, user->getNickname(), 
                                              channel->getChannelName() + " :No topic is set");
        server.sendToUser(user, response);
    } else {
        std::string response = generateResponse("localhost", 332, user->getNickname(), 
                                              channel->getChannelName() + " :" + topic);
        server.sendToUser(user, response);
    }
}

void CommandHandler::setTopic(User* user, Channel* channel, const std::string& newTopic, Server& server) {
    std::string serverResponse;

    if (channel->isTopicProtected()) {
        if (!channel->isChanOp(user)) {
            serverResponse = generateResponse("localhost", 482, user->getNickname(), 
                                             channel->getChannelName() + " :You're not channel operator");
            return server.sendToUser(user, serverResponse);
        }
    }
    channel->setTopic(newTopic);

    // Broadcast topic change to all channel members
    std::string topicMsg = ":" + user->getNickname() + "!" + user->getUsername() + 
                          "@localhost TOPIC " + channel->getChannelName() + " :" + newTopic + "\r\n";
    
    broadcastToChannelExceptSender(channel, topicMsg, user, server);

    std::cout << "Topic for channel '" << channel->getChannelName() 
              << "' set to: '" << newTopic << "' by " << user->getNickname() << std::endl;
}


// ==================== KICK COMMAND ====================
void CommandHandler::handleKICK(User* user, const std::vector<std::string>& params, Server& server) {
    std::string serverResponse;

    // Basic validation
    if (!user->isAuthenticated()) {
        serverResponse = generateResponse("localhost", 451, user->getNickname(), "You have not registered");
        return server.sendToUser(user, serverResponse);
    }
    
    if (params.size() < 3) {
        serverResponse = generateResponse("localhost", 461, user->getNickname(), "KICK :Not enough parameters");
        return server.sendToUser(user, serverResponse);
    }

    std::string channelName = params[1];
    std::string targetNick = params[2];
    std::string reason = "Kicked";

    if (params.size() > 3) {
        reason = params[3];
        if (reason.empty() || reason[0] != ':') {
            serverResponse = generateResponse("localhost", 461, user->getNickname(), "KICK :Syntax error - reason must start with colon");
            return server.sendToUser(user, serverResponse);
        }
        
        if (reason[0] == ':') {
            reason = reason.substr(1);
        }
    } else {
        reason = "Kicked by " + user->getNickname();
    }

    if (user->getNickname() == targetNick) {
        serverResponse = generateResponse("localhost", 482, user->getNickname(), "You cannot kick yourself");
        return server.sendToUser(user, serverResponse);
    }

    // Find the channel
    Channel* channel = Channel::findChannel(server.getChannels(), channelName);
    if (!channel) {
        serverResponse = generateResponse("localhost", 403, user->getNickname(), channelName + " :No such channel");
        return server.sendToUser(user, serverResponse);
    }

    // Check if user is in the channel
    if (!channel->hasUser(user)) {
        serverResponse = generateResponse("localhost", 442, user->getNickname(), channelName + " :You're not on that channel");
        return server.sendToUser(user, serverResponse);
    }

    // Check if user is operator
    if (!channel->isChanOp(user)) {
        serverResponse = generateResponse("localhost", 482, user->getNickname(), channelName + " :You're not channel operator");
        return server.sendToUser(user, serverResponse);
    }

    // Find target user
    User* targetUser = server.getUserByNickname(targetNick);
    if (!targetUser) {
        serverResponse = generateResponse("localhost", 401, user->getNickname(), targetNick + " :No such nick/channel");
        return server.sendToUser(user, serverResponse);
    }

    // Check if target is in channel
    if (!channel->hasUser(targetUser)) {
        serverResponse = generateResponse("localhost", 441, user->getNickname(), targetNick + " " + channelName + " :They aren't on that channel");
        return server.sendToUser(user, serverResponse);
    }

    // Create KICK message
    std::string kickMsg = ":" + user->getNickname() + "!" + user->getUsername() + 
                         "@localhost KICK " + channelName + " " + targetNick + " :" + reason + "\r\n";

    // Broadcast to all channel members
    broadcastToChannel(channel, kickMsg, server);

    // Remove target from channel
    if (channel->isChanOp(targetUser)) {
        channel->removeChanOp(targetUser);
    } else {
        channel->removeRegularUser(targetUser);
    }

    std::cout << "User '" << targetNick << "' kicked from channel '" << channelName 
              << "' by '" << user->getNickname() << "' for reason: " << reason << std::endl;
}


//==================== MODE COMMAND ====================
void CommandHandler::handleMODE(User* user, const std::vector<std::string>& params, Server& server) {
    std::string serverResponse;

    if (!user->isAuthenticated()) {
        serverResponse = generateResponse("localhost", 451, user->getNickname(), "You have not registered");
        return server.sendToUser(user, serverResponse);
    }
    
    if (params.size() < 2) {
        serverResponse = generateResponse("localhost", 461, user->getNickname(), "MODE :Not enough parameters");
        return server.sendToUser(user, serverResponse);
    }

    std::string target = params[1];

    // Only channel modes are supported for now
    if (target[0] == '#' || target[0] == '&') {
        handleChannelMode(user, params, server);
    } else {
        // User mode - not implemented
        serverResponse = generateResponse("localhost", 501, user->getNickname(), ":Unknown MODE flag");
        server.sendToUser(user, serverResponse);
    }
}

void CommandHandler::handleChannelMode(User* user, const std::vector<std::string>& params, Server& server) {
    std::string serverResponse;
    std::string channelName = params[1];

    Channel* channel = Channel::findChannel(server.getChannels(), channelName);
    if (!channel) {
        serverResponse = generateResponse("localhost", 403, user->getNickname(), channelName + " :No such channel");
        return server.sendToUser(user, serverResponse);
    }

    if (!channel->hasUser(user)) {
        serverResponse = generateResponse("localhost", 442, user->getNickname(), channelName + " :You're not on that channel");
        return server.sendToUser(user, serverResponse);
    }

    // Show current modes if no mode string provided
    if (params.size() == 2) {
        std::string modes = "+";
        if (channel->isInviteOnly()) modes += "i";
        if (channel->isTopicProtected()) modes += "t";
        if (!channel->getPassword().empty()) modes += "k";
        if (channel->getUserLimit() > 0) modes += "l";
        
        if (modes == "+") modes = "+t"; // Default modes
        
        serverResponse = generateResponse("localhost", 324, user->getNickname(), 
                                         channelName + " " + modes);
        server.sendToUser(user, serverResponse);
        return;
    }

    // Check if user is operator for mode changes
    if (!channel->isChanOp(user)) {
        serverResponse = generateResponse("localhost", 482, user->getNickname(), channelName + " :You're not channel operator");
        return server.sendToUser(user, serverResponse);
    }

    // Process mode changes - NEW IMPLEMENTATION
    processModeChanges(user, channel, params[2], params, server);
}

void CommandHandler::processModeChanges(User* user, Channel* channel, const std::string& modeString, 
                                      const std::vector<std::string>& params, Server& server) {
    bool adding = true;
    int paramIndex = 3;

    for (size_t i = 0; i < modeString.length(); i++) {
        char mode = modeString[i];
        
        if (mode == '+') {
            adding = true;
            continue;
        } else if (mode == '-') {
            adding = false;
            continue;
        }
        processSingleMode(user, channel, mode, adding, params, paramIndex, server);
    }
}

void CommandHandler::processSingleMode(User* user, Channel* channel, char mode, bool adding,
                                     const std::vector<std::string>& params, int& paramIndex, Server& server) {
    std::string modeChange = "";
    std::string modeParam = "";

    switch (mode) {
        case 'o': {
            // Operator privilege
            if (paramIndex < (int)params.size()) {
                std::string targetNick = params[paramIndex];
                paramIndex++;
                
                User* targetUser = server.getUserByNickname(targetNick);
                if (targetUser && channel->hasUser(targetUser)) {
                    if (adding) {
                        if (!channel->isChanOp(targetUser)) {
                            channel->grantOpPrivilege(targetUser);
                            modeChange = "+o";
                            modeParam = targetNick;
                        }
                    } else {
                        if (channel->isChanOp(targetUser)) {
                            channel->revokeOpPrivilege(targetUser);
                            modeChange = "-o";
                            modeParam = targetNick;
                        }
                    }
                }
            }
            break;
        }
        case 'i': {
            // Invite-only mode
            if (channel->isInviteOnly() != adding) {
                channel->setInviteOnly(adding);
                modeChange = adding ? "+i" : "-i";
            }
            break;
        }
        case 't': {
            // Topic protection mode
            if (channel->isTopicProtected() != adding) {
                channel->setTopicProtected(adding);
                modeChange = adding ? "+t" : "-t";
            }
            break;
        }
        case 'k': {
            // Channel key/password
            if (adding) {
                if (paramIndex < (int)params.size()) {
                    std::string key = params[paramIndex];
                    paramIndex++;
                    channel->setPassword(key);
                    modeChange = "+k";
                    channel->setPrivateCh(true);
                    modeParam = key;
                }
            } else {
                channel->setPassword("");
                channel->setPrivateCh(false);
                modeChange = "-k";
            }
            break;
        }
        case 'l': {
            // User limit
            if (adding) {
                if (paramIndex < (int)params.size()) {
                    std::string limitStr = params[paramIndex];
                    paramIndex++;
                    int limit = atoi(limitStr.c_str());
                    if (limit > 0) {
                        channel->setUserLimit(limit);
                        modeChange = "+l";
                        modeParam = limitStr;
                    }
                }
            } else {
                channel->setUserLimit(0);
                modeChange = "-l";
            }
            break;
        }
        default: {
            std::string serverResponse = generateResponse("localhost", 472, user->getNickname(), 
                                                        std::string(1, mode) + " :is unknown mode char to me");
            server.sendToUser(user, serverResponse);
            return;
        }
    }

    // Broadcast individual mode change if it occurred
    if (!modeChange.empty()) {
        std::string modeMsg = ":" + user->getNickname() + "!" + user->getUsername() + 
                             "@localhost MODE " + channel->getChannelName() + " " + modeChange;
        if (!modeParam.empty()) {
            modeMsg += " " + modeParam;
        }
        modeMsg += "\r\n";
        
        broadcastToChannel(channel, modeMsg, server);
        
        // Optional: Log the mode change
        std::cout << "Mode change broadcast: " << modeMsg;
    }
}


// ==================== INVITE COMMAND ====================
void CommandHandler::handleINVITE(User* user, const std::vector<std::string>& params, Server& server) {
    std::string serverResponse;

    // Basic validation
    if (!user->isAuthenticated()) {
        serverResponse = generateResponse("localhost", 451, user->getNickname(), "You have not registered");
        return server.sendToUser(user, serverResponse);
    }
    
    if (params.size() < 3) {
        serverResponse = generateResponse("localhost", 461, user->getNickname(), "INVITE :Not enough parameters");
        return server.sendToUser(user, serverResponse);
    }

    std::string targetNick = params[1];
    std::string channelName = params[2];

    // Find the channel
    Channel* channel = Channel::findChannel(server.getChannels(), channelName);
    if (!channel) {
        serverResponse = generateResponse("localhost", 403, user->getNickname(), channelName + " :No such channel");
        return server.sendToUser(user, serverResponse);
    }

    // Check if user is in the channel
    if (!channel->hasUser(user)) {
        serverResponse = generateResponse("localhost", 442, user->getNickname(), channelName + " :You're not on that channel");
        return server.sendToUser(user, serverResponse);
    }

    // Check if user is operator (only operators can invite in invite-only channels)
    if (channel->isInviteOnly() && !channel->isChanOp(user)) {
        serverResponse = generateResponse("localhost", 482, user->getNickname(), channelName + " :You're not channel operator");
        return server.sendToUser(user, serverResponse);
    }

    // Find target user
    User* targetUser = server.getUserByNickname(targetNick);
    if (!targetUser) {
        serverResponse = generateResponse("localhost", 401, user->getNickname(), targetNick + " :No such nick/channel");
        return server.sendToUser(user, serverResponse);
    }

    // Check if target is already in channel
    if (channel->hasUser(targetUser)) {
        serverResponse = generateResponse("localhost", 443, user->getNickname(), 
                                         targetNick + " " + channelName + " :is already on channel");
        return server.sendToUser(user, serverResponse);
    }

    // Check if channel has user limit and is full
    if (channel->getUserLimit() > 0) {
        int currentUsers = channel->getChanOps().size() + channel->getRegularUsers().size();
        if (currentUsers >= channel->getUserLimit()) {
            serverResponse = generateResponse("localhost", 471, user->getNickname(), 
                                             channelName + " :Cannot invite to channel (+l) - channel is full");
            return server.sendToUser(user, serverResponse);
        }
    }

    // Add user to invited list
    channel->getInvitedUsers().push_back(targetUser);

    // Send INVITE message to target user
    std::string inviteMsg = ":" + user->getNickname() + "!" + user->getUsername() + 
                           "@localhost INVITE " + targetNick + " " + channelName + "\r\n";
    server.sendToUser(targetUser, inviteMsg);

    // Send confirmation to inviting user
    serverResponse = generateResponse("localhost", 341, user->getNickname(), 
                                     targetNick + " " + channelName);
    server.sendToUser(user, serverResponse);


    std::cout << "User '" << user->getNickname() << "' invited '" << targetNick 
              << "' to channel '" << channelName << "'" << std::endl;
}

// ==================== BROADCAST HELPER FUNCTIONS ====================

void CommandHandler::broadcastToChannel(Channel* channel, const std::string& message, Server& server) {
    // Send to channel operators
    std::vector<User*>& chanOps = channel->getChanOps();
    for (std::vector<User*>::iterator it = chanOps.begin(); it != chanOps.end(); ++it) {
        server.sendToUser(*it, message);
    }

    // Send to regular users
    std::vector<User*>& regularUsers = channel->getRegularUsers();
    for (std::vector<User*>::iterator it = regularUsers.begin(); it != regularUsers.end(); ++it) {
        server.sendToUser(*it, message);
    }
}

void CommandHandler::broadcastToChannelExceptSender(Channel* channel, const std::string& message, 
                                                   User* sender, Server& server) {
    // Send to channel operators
    std::vector<User*>& chanOps = channel->getChanOps();
    for (std::vector<User*>::iterator it = chanOps.begin(); it != chanOps.end(); ++it) {
        if (*it != sender) {
            server.sendToUser(*it, message);
        }
    }

    // Send to regular users
    std::vector<User*>& regularUsers = channel->getRegularUsers();
    for (std::vector<User*>::iterator it = regularUsers.begin(); it != regularUsers.end(); ++it) {
        if (*it != sender) {
            server.sendToUser(*it, message);
        }
    }
}


void CommandHandler::handleUKNOWN(User* user, const std::vector<std::string>& params, Server& server) {
    std::string serverResponse;

    if (params.empty()) {
        serverResponse = generateResponse("localhost", 421, user->getNickname() + "<EMPTY>", "Unknown command");
    } else {
        serverResponse = generateResponse("localhost", 421, user->getNickname() + " " + params[0], "Unknown command");
    }
    server.sendToUser(user, serverResponse);
}
