#ifndef CHANNEL_IRC_HPP
# define CHANNEL_IRC_HPP

# include "Include.hpp"

class ClientIRC;
class ChannelManager;

class ChannelIRC {
    public:
        ChannelIRC(std::string, ChannelManager *, ClientIRC *);
        ~ChannelIRC();

        void SetName(std::string);
        std::string GetName();

        void SetTopic(std::string);
        std::string GetTopic();

        void SetMaxClients(int);
        unsigned int GetMaxClients();

        void SetDontTalk(std::string, bool);
        bool GetDontTalk(std::string);

        void SetModerated(bool);
        bool GetModerated();

        void SetPatron(ClientIRC *);
        ClientIRC *GetPatron();

        std::vector<ClientIRC *> GetClients();
        bool HasClient(ClientIRC *);
        void SendMessage(std::string, ClientIRC *);

        void AddClient(ClientIRC *);


    private:
        std::string                 _name;
        ChannelManager *            _channelManager;
        std::string                 _sujet;
        unsigned int                _maxClients;
        bool                        _moderated;
        std::vector<ClientIRC *>    _clients;
        std::map<std::string, bool> _dontTalk;
        ClientIRC *                 _patron;
};

#endif