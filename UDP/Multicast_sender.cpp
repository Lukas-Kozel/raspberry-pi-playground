#include "Multicast_sender.h"

MulticastSender::MulticastSender(const std::string& multicast_ip, int port)
    : UDPClient(multicast_ip, port) {}

void MulticastSender::sendMulticast(const std::vector<uint8_t>& message) {
    send(message);
}
