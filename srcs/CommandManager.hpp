#ifndef COMMANDMANAGER_HPP
# define COMMANDMANAGER_HPP

# include "Include.hpp"

class ChannelManager;
class ClientIRC;
class ServerIRC;

class CommandManager {
    typedef  std::vector<std::string>::iterator iterator;
    public:
        CommandManager(ChannelManager *,ServerIRC *);
        ~CommandManager();

        void Execute(ClientIRC *, std::string);
        void Pass(ClientIRC *, std::vector<std::string>);
        void Nick(ClientIRC *, std::vector<std::string>);
        void User(ClientIRC *, std::vector<std::string>);
        void Ping(ClientIRC *, std::vector<std::string>);
        void PrivMSG(ClientIRC *, std::vector<std::string>);
        void Join(ClientIRC *, std::vector<std::string>);



    private:
        ChannelManager * _channelManager;
        ServerIRC *_server;
};

#endif