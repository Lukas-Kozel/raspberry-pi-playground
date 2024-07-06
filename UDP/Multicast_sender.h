#ifndef MULTICASTSENDER_H
#define MULTICASTSENDER_H
#pragma once
#include "client.h"

class MulticastSender : public UDPClient {
public:
    MulticastSender(const std::string& multicast_ip, int port);
    void sendMulticast(const std::vector<uint8_t>& message);
};

#endif // MULTICASTSENDER_H
