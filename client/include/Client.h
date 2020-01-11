#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <ifc.h>
#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string> 
#include <vector>
#include <Config.h>
#define MAX 80 
#define SA struct sockaddr 

class Client{
    public:
        Client(std::string ip, uint32_t port);
        Client(const Client&) = delete;
        Client& operator=(const Client& ) = delete;

        ~Client();

        bool startClient();
        int sendData(std::vector<std::string> msg, uint32_t clientId);
    private:
        int         m_fd;
        std::string m_ip;
        uint32_t    m_port;
};

#endif
