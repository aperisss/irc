#include "ChannelIRC.hpp"

ChannelIRC::ChannelIRC(std::string name, ChannelManager *ChannelManager ,ClientIRC *patron): _name(name), _channelManager(ChannelManager) , _maxClients(0), _moderated(false), _patron(patron) {}

ChannelIRC::~ChannelIRC() {}

void ChannelIRC::SetName(std::string name) {
    _name = name;
}

std::string ChannelIRC::GetName() {
    return _name;
}

void ChannelIRC::SetTopic(std::string sujet) {
    _sujet = sujet;
}

std::string ChannelIRC::GetTopic() {
    return _sujet;
}


std::vector<ClientIRC *> ChannelIRC::GetClients() {
    return _clients;
}

void ChannelIRC::SetMaxClients(int maxClients) {
    _maxClients = maxClients;
}

unsigned int ChannelIRC::GetMaxClients() {
    return _maxClients;
}

void ChannelIRC::SetModerated(bool moderated) {
    _moderated = moderated;
}

bool ChannelIRC::GetModerated() {
    return _moderated;
}

void ChannelIRC::SetPatron(ClientIRC *patron) {
    _patron = patron;
}

ClientIRC *ChannelIRC::GetPatron() {
    return _patron;
}

/**
 * @brief Ajoute un client a la liste des clients qui ne peuvent pas parler
 * 
 * @param silent 
 * @param value 
 */
void ChannelIRC::SetDontTalk(std::string silent, bool value) {
    _dontTalk[silent] = value;
}

bool ChannelIRC::GetDontTalk(std::string silent) {
    return _dontTalk[silent];
}

/**
 * @brief Plymorphisme pour envoyer un message a tous les clients du channel
 * Obligé de refaire la fonction car on ne peut pas envoyer de message a un client sans le pointer
 * 
 * @param message 
 * @param client 
 */
void ChannelIRC::SendMessage(std::string message, ClientIRC *client) {
    for (std::vector<ClientIRC *>::iterator it = _clients.begin(); it != _clients.end(); it++) {
        if ((*it)->GetKilled() || *it == client) continue;
        (*it)->SendMessage(message);
    }
}

/**
 * @brief Verifie si un client est dans le channel
 * 
 * @param client 
 * @return true 
 * @return false 
 */
bool ChannelIRC::HasClient(ClientIRC *client) {
    for (std::vector<ClientIRC *>::iterator it = _clients.begin(); it != _clients.end(); it++) {
        if (!client->GetKilled() && *it == client) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Ajoute un client au channel
 *  Envoie les messages de join, de topic, de names, etc...
 * J'ai gardé la syntaxe de irssi qui fera donc la meme chose sur nc
 * @param client 
 */
void ChannelIRC::AddClient(ClientIRC *client) {
    _clients.push_back(client);

    std::string nameList = "";
    for (std::vector<ClientIRC *>::iterator it = _clients.begin(); it != _clients.end(); it++) {
        std::string prefix = "";
        if ((*it) == _patron)
            prefix += "@";
        else if (GetDontTalk((*it)->GetNick()))
            prefix += "+";
        nameList += prefix + (*it)->GetNick() + " ";
    }
    nameList = nameList.substr(0, nameList.size() - 1);

    this->SendMessage(":" + client->GetNick() + "!user@host JOIN " + _name + "\r\n", NULL);
    client->SendMessage(":ircserver 353 " + client->GetNick() + " = " + _name + " :" + nameList + "\r\n");
    client->SendMessage(":ircserver 366 " + client->GetNick() + " " + _name + " :End of /NAMES list.\r\n");

    if (_sujet != "")
        client->SendMessage(":ircserver 332 " + client->GetNick() + " " + _name + " :" + _sujet + "\r\n");
    else
        client->SendMessage(":ircserver 331 " + client->GetNick() + " " + _name + " :No topic is set\r\n");
}