#ifndef MULTICASTRECEIVER_H
#define MULTICASTRECEIVER_H
#pragma once
#include "client.h"

class MulticastReceiver : public UDPClient {
public:
    MulticastReceiver(const std::string& multicast_ip, int port);
    void receiveMulticast(std::vector<uint8_t>& message);
};

#endif // MULTICASTRECEIVER_H
