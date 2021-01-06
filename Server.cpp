
#include "Server.h"
Server::Server(int port)throw (const char*) {
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd < 0)
    {
        throw system_error(errno, generic_category(), "Couldn't open a new socket");
    }
    serverAdress.sin_family = AF_INET;
    serverAdress.sin_addr.s_addr = INADDR_ANY;
    serverAdress.sin_port = htons(port);
    if (bind(socketFd, (const sockaddr *)&serverAdress, sizeof(serverAdress)) < 0)
    {
        throw system_error(errno, generic_category(), "Call to bind() failed");
    }
    if (listen(socketFd, 3) < 0)
    {
        throw system_error(errno, generic_category(), "Error when listening to new connections");
    }
}

void Server::start(ClientHandler& ch)throw(const char*){
    t = new thread([&ch,this](){
        unsigned a = 1;
        while (a != 3) {
            socklen_t clientAddressLength = sizeof(this->clientAddress);
            a = alarm(3);
            clientSocketFd = accept(socketFd, (sockaddr *) &clientAddress, &clientAddressLength);
            if (clientSocketFd < 0) {
                throw system_error(errno, generic_category(), "Error accepting new connection");
            }
            ch.handle(clientSocketFd);
        }
        close(clientSocketFd);
    });
}

void Server::stop(){
    t->join(); // do not delete this!
    close(socketFd);
}

Server::~Server() {
}

