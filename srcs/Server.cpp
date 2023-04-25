#include "Server.hpp"

ServerIRC::ServerIRC() {
}

ServerIRC::ServerIRC(int port, std::string s): _port(port), _running(true), _password(s) {
    std::cout << "ServerIRC::ServerIRC(int)" << std::endl;
    // Création de la socket
    // Un socket est un point de communication entre 2 machines, il est composé d'un port et d'une adresse IP
    _sockfd = socket(AF_INET, SOCK_STREAM, 0); // AF_INET = IPv4, SOCK_STREAM = TCP, 0 est le protocol par defaut qui s'adapetera aux 2 premiers parametres
    if (_sockfd == -1) {
        perror("socket");
        exit(1);
    }

    //Une fois la socket créée, on peut la configurer
    //On va dire à la socket qu'elle peut être réutilisée rapidement après une fermeture
    //Cela permet de ne pas avoir à attendre que le port soit libéré par le système
    int yes = 1;
    if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");


    struct sockaddr_in server; // Structure qui contiendra les informations de la socket, notamment l'adresse IP et le port, # include <netinet/in.h>
    server.sin_family = AF_INET; // On précise que l'on utilise l'IPv4
    server.sin_port = htons(port); // On précise le port, en utilisant htons pour convertir le port en format réseau, ici TCP
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // On précise l'adresse IP, en utilisant inet_addr pour convertir l'adresse en format réseau
    //Va faire un loopback sur la machine elle meme, donc pas besoin de mettre l'adresse IP de la machine

    if (bind(_sockfd, (struct sockaddr *)&server, sizeof(server)) == -1) { // On bind/lie la socket sur l'adresse IP et le port précisé
        perror("bind");
        exit(1);
    }

    //Mise en ecoute de la socket, et de la limite de connexion soit 10
    if (listen(_sockfd, 10) == -1) {
        perror("listen");
        exit(1);
    }

    _channelManager = new ChannelManager();
    _commandManager = new CommandManager(_channelManager, this);
}

ServerIRC::~ServerIRC() {
    close(_sockfd);

    for (iterator it = _clients.begin(); it != _clients.end(); ++it) {
        delete (*it);
    }

    delete _channelManager;
    delete _commandManager;

    std::cout << "ServerIRC::~ServerIRC()" << std::endl;
}

int ServerIRC::getPort(void) const {
    return (_port);
}

std::string ServerIRC::getPassword(void) const {
    return (_password);
}

std::vector<ClientIRC *> ServerIRC::getClients() {
    return (_clients);
}

/**
 * @brief Cette fonction GetClientByNick parcourt la liste des clients connectés au serveur IRC
 *  et renvoie un pointeur vers le client ayant le surnom (nick) spécifié.
 * 
 * @param nick 
 * @return ClientIRC* 
 */
ClientIRC *ServerIRC::GetClientByNick(std::string nick) {
    for (iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (!(*it)->GetKilled() && (*it)->GetNick() == nick)
            return (*it);
    }
    return (NULL);
}

/**
 * @brief Cette fonction CreateClient() est utilisée pour créer un nouveau client lorsqu'un client se connecte au serveur. 
 * Elle utilise la fonction accept() pour accepter une nouvelle connexion sur la socket d'écoute _sockfd.
 * 
 * @return ClientIRC* 
 */
ClientIRC *ServerIRC::CreateClient() {
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);
    int clientfd = accept(_sockfd, (struct sockaddr *)&client, &client_len);

    if (clientfd == -1) {
        perror("accept");
        exit(1);
    }

    FD_SET(clientfd, &_currentSockets);// Ensuite, la fonction utilise FD_SET pour ajouter le nouveau descripteur de fichier à l'ensemble de descripteurs de fichiers _currentSockets que le serveur surveille.

    ClientIRC *client_irc = new ClientIRC(clientfd);
    _clients.push_back(client_irc);

    std::cout << "[" << client_irc->GetFd() << "] " << "Client connected" << std::endl;
    if (fcntl(client_irc->GetFd(), F_SETFL, fcntl(client_irc->GetFd(), F_GETFL) | O_NONBLOCK) < 0) {/*
     la fonction fcntl pour configurer le descripteur de fichier du client en mode non bloquant.
     Les sockets par défaut sont bloquantes, ce qui signifie que toute lecture ou écriture effectuée sur une socket
      sera bloquée jusqu'à ce que les données soient disponibles ou que la socket soit prête à écrire*/
        perror("fcntl");
        exit(1);
    }
    return client_irc;
}

void ServerIRC::RemoveClient(ClientIRC *client) {
    for (iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (!(*it)->GetKilled() && (*it)->GetFd() == client->GetFd()) { // On parcourt la liste des clients connectés au serveur IRC et on verifie si il n'est pas kill. Si il ne l'est pas, on le kill
            int fd = client->GetFd();
            close(fd);
            FD_CLR(fd, &_currentSockets); // On supprime le descripteur de fichier du client de l'ensemble de descripteurs de fichie assoicé au client et ses sockets.
            (*it)->SetKilled(true);
        }
    }
}

/**
 * @brief Coeur du serveur IRC, cette fonction Run() est appelée dans le main() et est la fonction principale du serveur.
 * 
 */
void ServerIRC::Run() {
    FD_ZERO(&_currentSockets); // On initialise l'ensemble de descripteurs de fichiers à 0
    FD_SET(_sockfd, &_currentSockets); // Ajoute le descripteur de fichier _sockfd (socket du serveur) à l'ensemble _currentSockets.

    struct timeval timeout; // Structure qui contient le temps d'attente avant de sortir de la fonction select
    timeout.tv_sec = 0; // On précise le temps d'attente en secondes
    timeout.tv_usec = 0; // On précise le temps d'attente en microsecondes

    while (keepRunning && _running) { // Tant que le serveur est en marche, on boucle, tant que keepRunning et _running sont vrai
        _readySockets = _currentSockets;
        if (select(FD_SETSIZE, &_readySockets, NULL, NULL, &timeout) < 0) { // On utilise la fonction select pour surveiller les sockets du serveur et des clients
            perror("select");
            exit(1);
        }

        for (int i = 0; i < FD_SETSIZE; i++) { // On parcourt l'ensemble de descripteurs de fichiers _readySockets
            if (FD_ISSET(i, &_readySockets)) {
                if (i == _sockfd) {
                    CreateClient(); // Si le descripteur de fichier est égal à la socket du serveur, on crée un nouveau client
                } else {
                    FD_CLR(i, &_currentSockets); // Sinon, on supprime le descripteur de fichier de l'ensemble de descripteurs de fichiers _currentSockets
                }
            }
        }

        for (iterator it = _clients.begin(); it != _clients.end(); ++it) { // On parcourt la liste des clients connectés au serveur IRC
            if ((*it)->GetKilled()) { // Si le client est kill, on le supprime de la liste des clients connectés au serveur IRC
                continue;
            }

            char buffer[1024]; // On crée un buffer de 1024 octets, pour stoker les données recues par le serveur
            std::string clientBuffer = (*it)->GetBuffer();

            int lenght = recv((*it)->GetFd(), buffer, sizeof(buffer), 0); // On utilise la fonction recv pour lire les données envoyées par le client
            if (!lenght) {
                std::cout << "Client disconnected without a QUIT" << std::endl;
                // Si le client se déconnecte sans envoyer de QUIT, on envoie un QUIT au client via command manager
                exit(1);
            }
            if (lenght > 0) {
                buffer[lenght] = '\0'; // On ajoute un caractère de fin de chaine de caractère
                fflush(stdout); // On vide le buffer, et on force l'affichage des données recues par le serveur
                clientBuffer += buffer; // On ajoute les données recues par le serveur au buffer du client
                (*it)->SetBuffer(clientBuffer); // On met à jour le buffer du client
            } else if (clientBuffer[clientBuffer.size() - 1] == '\n') { // Si le client a envoyé un message, on l'envoie au serveur
                (*it)->SetBuffer(""); // On met à jour le buffer du client
                MesssageReceived((*it) ,clientBuffer); // On appelle la fonction MesssageReceived pour traiter le message envoyé par le client
            }
            buffer[0] = '\0'; // Enfin on met à jour le buffer
        }
    }
}

/**
 * @brief Cette fonction permet de fermer le serveur IRC
 * 
 */
void ServerIRC::Close() {
    close(_sockfd);
    for (iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (!(*it)->GetKilled()) {
            RemoveClient((*it));
        }
    }
    _running = false;
}

/**
 * @brief Cette fonction permet de traiter les messages envoyés par les clients
 * 
 * @param client 
 * @param message 
 */
void ServerIRC::MesssageReceived(ClientIRC *client ,std::string message) {
    std::vector<std::string> messages = splitString(message, "\n");
    for (std::vector<std::string>::iterator it = messages.begin(); it != messages.end(); ++it) {
        this->_commandManager->Execute(client, *it);
        if (client->GetKilled()) {
            break;
        }
    }
}

/**
 * @brief Compte le nombre de clients actifs connectés au serveur IRC
 * 
 * @return int 
 */
int ServerIRC::getClientSize() {
    int size = 0;
    for (iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (!(*it)->GetKilled()) {
            size++;
        }
    }
    return size;
}