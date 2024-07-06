#include "client.h"
#include <iostream>
#include <cstring>

UDPClient::UDPClient(const std::string& ip, const int port) {
    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Socket creation failed\n";
        exit(EXIT_FAILURE);
    }

    // Set SO_REUSEADDR to allow multiple sockets to bind to the same port
    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0) {
        std::cerr << "Setting SO_REUSEADDR failed\n";
        exit(EXIT_FAILURE);
    }

    // Clean up the memory
    std::memset(&addr, 0, sizeof(addr));

    // Assign IP and PORT
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    // Bind the socket
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Bind failed: " << strerror(errno) << "\n";
        exit(EXIT_FAILURE);
    } else {
        std::cout << "Socket successfully bound\n";
    }

    // Join the multicast group
    mreq.imr_multiaddr.s_addr = inet_addr(ip.c_str());
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        std::cerr << "Adding multicast group failed: " << strerror(errno) << "\n";
        exit(EXIT_FAILURE);
    } else {
        std::cout << "Joined multicast group\n";
    }
}

UDPClient::~UDPClient() {
    setsockopt(sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
    close(sockfd);
}

void UDPClient::send(const std::vector<uint8_t>& message) {
    sendto(sockfd, message.data(), message.size(), 0, (struct sockaddr*)&addr, sizeof(addr));
}

void UDPClient::receive(std::vector<uint8_t>& buffer) {
    char temp_buffer[1024];
    ssize_t received_msg = recvfrom(sockfd, temp_buffer, sizeof(temp_buffer), 0, nullptr, nullptr);
    if (received_msg > 0) {
        buffer.assign(temp_buffer, temp_buffer + received_msg);
    } else {
        std::cerr << "Receive failed or no data available: " << strerror(errno) << "\n";
    }
}
