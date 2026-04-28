#pragma once
#include "User.hpp"
#include "Server.hpp"

class CommandHandler{
    private:
        // Authentication
        static void handlePASS(User* user, const std::vector<std::string>& params, Server& server);
        static void handleUSER(User* user, const std::vector<std::string>& params, Server& server);
        static void handleNICK(User* user, const std::vector<std::string>& params, Server& server);
        static void handlePING(User* user, const std::vector<std::string>& params, Server& server);
        // Channel Commands
        static void handleJOIN(User* user, const std::vector<std::string>& params, Server& server);
        static void handlePART(User* user, const std::vector<std::string>& params, Server& server);
        static void handlePRIVMSG(User* user, const std::vector<std::string>& params, Server& server);
        static void handleQUIT(User* user, const std::vector<std::string>& params, Server& server);
        static void handleTOPIC(User* user, const std::vector<std::string>& params, Server& server);
        static void handleINVITE(User* user, const std::vector<std::string>& params, Server& server);
        static void handleKICK(User* user, const std::vector<std::string>& params, Server& server);
        static void handleMODE(User* user, const std::vector<std::string>& params, Server& server);
        static void handleUKNOWN(User* user, const std::vector<std::string>& params, Server& server);
        // Utils
        static std::string generateResponse(const std::string& serverName, int statusCode,
            const std::string& target, const std::string& message);
        static void handleChannelPRIVMSG(User* user, const std::string& channelName, const std::string& message, Server& server);
        static void handlePrivatePRIVMSG(User* user, const std::string& targetNick, const std::string& message, Server& server);
        static void broadcastToChannelExceptSender(Channel* channel, const std::string& message, User* sender, Server& server);
        static void displayTopic(User* user, Channel* channel, Server& server);
        static void setTopic(User* user, Channel* channel, const std::string& newTopic, Server& server);
        static void handleChannelMode(User* user, const std::vector<std::string>& params, Server& server);
        static void broadcastToChannel(Channel* channel, const std::string& message, Server& server);
        static void processModeChanges(User* user, Channel* channel, const std::string& modeString, const std::vector<std::string>& params, Server& server);
        static void processSingleMode(User* user, Channel* channel, char mode, bool adding, const std::vector<std::string>& params, int& paramIndex, Server& server);

    public:
        static std::vector<std::string> parseCommand(const std::string& cmd);
        static void handleCommand(User* user, const std::string& cmd, Server& server);
};
