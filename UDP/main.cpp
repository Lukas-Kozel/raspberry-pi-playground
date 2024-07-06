#include "Multicast_sender.h"
#include "msg/heartbeat.h"
#include <chrono>
#include <thread>
#include <vector>

int main() {
    const std::string multicast_ip = "224.0.0.1";
    const int port = 8081;

    MulticastSender sender(multicast_ip, port);
    Heartbeat heartbeat(0, 0, 0, 0);

    while (true) {
        heartbeat.update_timestamp();
        std::vector<uint8_t> heartbeatMessage(heartbeat.size());
        heartbeat.serialize(heartbeatMessage.data());
        sender.sendMulticast(heartbeatMessage);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    return 0;
}
