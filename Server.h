
#ifndef SERVER_H_
#define SERVER_H_
#include <thread>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <signal.h>
#include "CLI.h"

#define READING_BUFF_SIZE 2048
using namespace std;

// edit your ClientHandler interface here:
class ClientHandler{
public:
    virtual void handle(int clientID)=0;
};

class AnomalyDetectionHandler:public ClientHandler{
public:
    virtual void handle(int clientID){
        char buffer[READING_BUFF_SIZE];
        bzero(buffer, READING_BUFF_SIZE);
        SocketIO dio(clientID);
        CLI cli(&dio);
        cli.start();
    }
};

class Server {
    int socketFd;
    int clientSocketFd;
    struct sockaddr_in serverAdress;
    struct sockaddr_in clientAddress;
    thread* t; // the thread to run the start() method in

public:
    Server(int port) throw (const char*);
    virtual ~Server();
    void start(ClientHandler& ch)throw(const char*);
    void stop();
};

#endif /* SERVER_H_ */
