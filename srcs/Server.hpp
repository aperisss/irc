#ifndef SERVER_HPP
# define SERVER_HPP

#include "Include.hpp"

class ChannelManager;
class CommandManager;
class ClientIRC;

class ServerIRC {

    typedef std::vector<ClientIRC* >::iterator iterator;

    public:
        ServerIRC();
        ServerIRC(int, std::string);
        ~ServerIRC();


        int				getPort(void) const;
		std::string		getPassword(void) const;
        void Run();
        void Close();
        void MesssageReceived(ClientIRC * ,std::string);
        void ExecuteCommand(ClientIRC *, std::string);
        ClientIRC *CreateClient();
        std::vector<ClientIRC *> getClients();
        int getClientSize();
        ClientIRC *GetClientByNick(std::string);
        void RemoveClient(ClientIRC *);
    
    private:
        int _port;
        int _sockfd;
        bool _running;
        std::string _password;
        fd_set _currentSockets;
        fd_set _exceptSockets;
        fd_set _readySockets;
        CommandManager *_commandManager;
        ChannelManager *_channelManager;
        /*vector of client*/
        std::vector <ClientIRC *> _clients;
};
#endif