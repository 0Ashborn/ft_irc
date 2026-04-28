#pragma once
#include "User.hpp"
#include "Server.hpp"

class Server;

class Channel {
    private:
        std::string chName;
        std::string password;
        std::string topic;
        bool inviteOnly;
        bool privateCh;
        bool topicProtected;  // i Added this for +t mode
        int userLimit; // and this for +l mode 
        std::vector<User*> regularUsers;
        std::vector<User*> chanOps;
        std::vector<User*> invitedUsers;
        
        User* findRegularUser(User* user);
        User* findChanop(User* user);
        // Server related
        void sendJoinMessage(Server& server, User* joinedUser);
        void sendTopic(Server& server, User* joinedUser);
        void sendChannelUsers(Server& server, User* joinedUser);

    public:
        Channel(std::string chName, std::string password = "", std::string topic = "");
        void broadcastJoinMessage(Server& server, User* joinedUser);
        static Channel* findChannel(const std::vector<Channel*> channels, const std::string& channel);

        static bool isValidChannelName(std::string channelName);
        bool isChanOp(User* user);
        bool isInviteOnly() const;
        bool isPrivateCh() const;
        bool isUserInvited(User* user);
        bool isTopicProtected() const;  // mine
        int getUserLimit() const;       // mine
        bool hasUser(User* user);

        std::vector<User*>& getRegularUsers();
        std::vector<User*>& getChanOps();
        std::vector<User*>& getInvitedUsers();
        std::string getPassword() const;
        std::string getChannelName() const;
        
        void addChanOp(User* user);
        void addRegularUser(User* user);
        void removeRegularUser(User* user);
        void removeChanOp(User* user);

        void grantOpPrivilege(User* user);
        void revokeOpPrivilege(User* user);

        void setTopicProtected(bool protect);  // mine
        void setUserLimit(int limit);               // mine
        void setPassword(const std::string& newPassword);  // mine
        void setInviteOnly(bool invite);            // mine
        void setPrivateCh(bool priv);               // mine

        void setTopic(const std::string& newTopic);  // mine
        std::string getTopic() const;                  // mine
};
