#include "client.h"


UDPClient::UDPClient(const std::string& ip, const int port){
        // socket create and verification
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    //cleanup the memory
    std::memset(&addr, 0, sizeof(addr));
        // assign IP, PORT
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);

        // connect the client socket to server socket
    if (connect(sockfd, (struct sockaddr*) &addr, sizeof(addr)) != 0) {
        std::cerr << "connection with the server failed...\n";
        exit(EXIT_FAILURE);
    } else {
        std::cout << "connected to the server..\n";
    }
}

UDPClient::~UDPClient() {
        close(sockfd);
    }

void UDPClient::send(const std::vector<uint8_t> &message)
    {
        sendto(sockfd,message.data(),message.size(),0,(struct sockaddr*) &addr,sizeof(addr));
    }
void UDPClient::receive(std::vector<uint8_t> &buffer)
    {
        char temp_buffer[MAX];
        ssize_t recevied_msg = recvfrom(sockfd,temp_buffer,sizeof(temp_buffer),0,nullptr,nullptr); // not unsigned, because it throws -1 when error
        if(recevied_msg > 0){
            buffer.assign(temp_buffer,temp_buffer+recevied_msg);
        }
    }