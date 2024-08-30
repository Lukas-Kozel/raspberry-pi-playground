#ifndef MULTICASTRECEIVER_H
#define MULTICASTRECEIVER_H
#pragma once
#include "client.h"
#include <functional>
#include <thread>

class MulticastReceiver : public UDPClient {
public:
    MulticastReceiver(const std::string& multicast_ip, int port);
    void receiveMulticast(std::vector<uint8_t>& message);
    void registerCallback(std::function<void(const std::vector<uint8_t>&)> callback);
    void listen_for_message();

private:
    std::function<void(const std::vector<uint8_t>&)> callback;
    std::thread listening_thread;
    MulticastReceiver();
    ~MulticastReceiver();
};

#endif // MULTICASTRECEIVER_H
