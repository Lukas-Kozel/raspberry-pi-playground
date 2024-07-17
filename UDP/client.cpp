// basically interface or base class for UDP communication

#include "client.h"
#include <iostream>
#include <cstring>

UDPClient::UDPClient(const std::string& ip, const int port) {
    multicast_ip = ip;
    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Socket creation failed: " << strerror(errno) << "\n";
        exit(EXIT_FAILURE);
    }

    // Set SO_REUSEADDR to allow multiple sockets to bind to the same port
    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        std::cerr << "Setting SO_REUSEADDR failed: " << strerror(errno) << "\n";
        exit(EXIT_FAILURE);
    }

    // Set the time-to-live for messages to 1 so they do not go past the local network segment
    int ttl = 1;
    if (setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0) {
        std::cerr << "Setting IP_MULTICAST_TTL failed: " << strerror(errno) << "\n";
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
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);  // Ensure this matches the interface used in Python
    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        std::cerr << "Adding multicast group failed: " << strerror(errno) << "\n";
        exit(EXIT_FAILURE);
    } else {
        std::cout << "Joined multicast group\n";
    }
}

UDPClient::~UDPClient() {
    if (setsockopt(sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        std::cerr << "Dropping multicast group failed: " << strerror(errno) << "\n";
    }
    close(sockfd);
}

void UDPClient::send(const std::vector<uint8_t>& message) {
    // Ensure the address is set correctly for sending
    addr.sin_addr.s_addr = inet_addr((char*)&multicast_ip);  // Multicast IP
    int n = sendto(sockfd, message.data(), message.size(), 0, (struct sockaddr*)&addr, sizeof(addr));
    if (n < 0) {
        std::cerr << "Send failed: " << strerror(errno) << "\n";
    } else {
        std::cout << "Sent message to multicast group\n";
    }
}

void UDPClient::receive(std::vector<uint8_t>& buffer) {
    char temp_buffer[1024];
    ssize_t received_msg = recvfrom(sockfd, temp_buffer, sizeof(temp_buffer), 0, nullptr, nullptr);
    if (received_msg > 0) {
        buffer.assign(temp_buffer, temp_buffer + received_msg);
        std::cout << "Received message from multicast group\n";
    } else {
        std::cerr << "Receive failed or no data available: " << strerror(errno) << "\n";
    }
}