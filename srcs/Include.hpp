#ifndef INCLUDE_HPP
# define INCLUDE_HPP
# include <iostream>
# include <cstdlib>
# include <cstring>
# include <string>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>

# include <sstream>
# include <poll.h>
# include <vector>
# include <map>
# include <signal.h>
# include <fcntl.h>
# include <stdio.h>

# include "Server.hpp"
# include "ClientIRC.hpp"
# include "CommandManager.hpp"
# include "ChannelIRC.hpp"
# include "ChannelManager.hpp"

std::vector<std::string> splitString(std::string str, std::string delimiter);
void startServer(int port, std::string password);

extern bool keepRunning;

#endif