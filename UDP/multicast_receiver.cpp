#include "multicast_receiver.h"

void MulticastReceiver::receiveMulticast(std::vector<uint8_t> &message)
{
    receive(message);
}

MulticastReceiver::MulticastReceiver(const std::string& multicast_ip, int port): UDPClient(multicast_ip,port){};