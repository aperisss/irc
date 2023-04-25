#include "Include.hpp"

bool keepRunning = true;

std::vector<std::string> splitString(std::string str, std::string delimiter) {
    std::vector<std::string> result;
    size_t pos = 0;
    std::string token;
    while ((pos = str.find(delimiter)) != std::string::npos) {
        token = str.substr(0, pos);
        result.push_back(token);
        str.erase(0, pos + delimiter.length());
    }
    if (!str.empty()) {
        result.push_back(str);
    }
    return result;
}

void startServer(int port, std::string password) {
    ServerIRC server(port, password);
    server.Run();
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return 1;
    }
    startServer(atoi(argv[1]), argv[2]);

    return 0;
}