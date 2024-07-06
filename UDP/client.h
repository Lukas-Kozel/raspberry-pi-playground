#pragma once
#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <iostream>
#include <cstdlib>
#include <cstring> // std::memset()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#include <vector>

#define MAX 1024

class UDPClient{
    protected:
    int sockfd;
    struct sockaddr_in addr;
    struct ip_mreq mreq;
    public:
    UDPClient(const std::string& ip,const int port);
    ~UDPClient();
    void send(const std::vector<uint8_t>& message); // message is expected to be byte array
    void receive(std::vector<uint8_t>& buffer);
};