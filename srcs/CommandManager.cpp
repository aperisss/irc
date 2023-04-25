#include "CommandManager.hpp"

CommandManager::CommandManager(ChannelManager * _channelManager,ServerIRC* _server): _channelManager(_channelManager), _server(_server) {}
CommandManager::~CommandManager() {}

std::string concatString(std::vector<std::string> str, int start) {
    std::string result = "";
    for (unsigned long i = start; i < str.size(); i++) {
        result += str[i];
        if (i != str.size() - 1) {
            result += " ";
        }
    }
    return result;
}

void CommandManager::Execute(ClientIRC *client, std::string command) {
    if (command.empty()) {
        return;
    }

    std::cout << "MESSAGE RECEIVED : " << command << std::endl;

    std::vector<std::string> args = splitString(command, " ");
    std::cout << "Command: " << args[0] << std::endl;


    if (args[args.size() - 1].find("\r") != std::string::npos) {
        args[args.size() - 1].erase(args[args.size() - 1].find("\r"), 1);
    }
    if (args[args.size() - 1].find("\n") != std::string::npos) {
        args[args.size() - 1].erase(args[args.size() - 1].find("\n"), 1);
    }
    
    std::string cmd = args[0];
    if (cmd == "PASS")
        this->Pass(client, args);
    else if (cmd == "NICK")
        this->Nick(client, args);
    else if (cmd == "USER")
        this->User(client, args);
    else if (cmd == "CAP"){
    }
    else if (!client->GetRegistered()) {
        client->SendMessage(":ircserver 451 :You have not registered\n");
        return;
    }
    else if (cmd == "PING")
        this->Ping(client, args);
    else if (cmd == "PRIVMSG")
        this->PrivMSG(client, args);
    else
    {
        client->SendMessage(":ircserver 421 " + cmd +  " :Unknown command\n");
    }
}

void CommandManager::Pass(ClientIRC *client, std::vector<std::string> args) {
    std::string password = concatString(args, 1);
    if (client->getPassword()) {
        client->SendMessage(":ircserver 462 :You may not reregister\n");
    }
    else if (args.size() < 2) {
        client->SendMessage(":ircserver 461 PASS :Not enough parameters\n");
    } 
    else if (password == this->_server->getPassword()) {
        client->setPassword(true);
    } 
    else {
        client->SendMessage(":ircserver 464 :Password incorrect\r\n");
        _server->RemoveClient(client);

    }
}

void CommandManager::Nick(ClientIRC *client, std::vector<std::string> args) {
    if (args.size() < 2) {
        client->SendMessage(":ircserver 461 " + client->GetNick() + " NICK :Not enough parameters\r\n");
        return;
    }

    ClientIRC *client2 = this->_server->GetClientByNick(args[1]);
    if (client2 != NULL) {
        client->SendMessage("Nickname already in use\r\n");
        if (!client->GetRegistered()) {
            _server->RemoveClient(client);
        }
        return;
    }
    std::string oldNick = client->GetNick();
    client->SetNick(args[1]);

    if (client->GetRegistered()) {
        client->SendMessage(":" + oldNick + "!user@host NICK " + client->GetNick() + "\r\n");;
    }
}

void CommandManager::User(ClientIRC *client, std::vector<std::string> args) {
    if (args.size() < 5) {
        client->SendMessage(":ircserver 461 " + client->GetNick() + " USER :Not enough parameters\r\n");
        return;
    }
    if (!client->getPassword()) {
        client->SendMessage(":ircserver 462 :You may not reregister\r\n");
        return;
    }
    std::string username = concatString(args, 4).erase(0, 1);
    client->SetUserName(args[1]);
    client->SetRealName(username);
    if(!client->getPassword()){
        client->SendMessage(":Wrong password\r\n");
        _server->RemoveClient(client);
        return;
    } else if(client->GetNick().empty()){
        client->SendMessage(":No nickname given\r\n");
        _server->RemoveClient(client);
        return;
    }
    client->SetRegistered(true);
    client->SendMessage(":ircserver 001 " + client->GetNick() + " :A t'on reussi ?\n");
}

void CommandManager::Ping(ClientIRC *client, std::vector<std::string> args) {
    std::string concat = "PONG " + concatString(args, 1) + "\r\n";
    client->SendMessage(concat);
    std::cout << concat << std::endl;
}

void CommandManager::PrivMSG(ClientIRC *client, std::vector<std::string> args) {
    if (args.size() < 3) {
        client->SendMessage(":ircserver 461 " + client->GetNick() + " PRIVMSG :Not enough parameters\n");
        return;
    }
    std::string message = concatString(args, 2);
    //partie Channel a ajouter
    ClientIRC *clientT = this->_server->GetClientByNick(args[1]);
    if (!clientT) {
        client->SendMessage(":ircserver 403 " + client->GetNick() + " " + args[1] + " :No such channel\r\n");
        return;
    } else {
        clientT->SendMessage(":" + client->GetNick() + " PRIVMSG " + args[1] + " " + message + "\r\n");
    }
}

/**
 * @brief C'est grave pas moi qui a fait Join je l'ai recuperer pour test les channels
 * 
 * @param client 
 * @param args 
 */
void CommandManager::Join(ClientIRC *client, std::vector<std::string> args) {
    if (args.size() < 2) {
        client->SendMessage(":ircserver 461 " + client->GetNick() + " JOIN :Not enough parameters\n");
        return;
    }

    std::vector<std::string> channels = splitString(args[1], ",");
    for (iterator it = channels.begin(); it != channels.end(); ++it) {
        ChannelIRC *channel = this->_channelManager->GetChannel((*it));
        if (channel) {
            if (channel->GetMaxClients() != 0 && channel->GetClients().size() >= channel->GetMaxClients()) {
                client->SendMessage(":ircserver 471 ahamdoun " + (*it) + " :Cannot join channel (+l)\r\n");
                continue;
            }
            channel->AddClient(client);
        } else {
            this->_channelManager->CreateChannel((*it), client);
        }
    }
}
