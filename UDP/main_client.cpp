//< raspberry Pi main app.

#include "Multicast_sender.h"
#include "msg/heartbeat.h"
#include <chrono>
#include <thread>
#include <vector>

int main() {
    const std::string multicast_ip = "224.0.0.1";
    const int port = 8081;

    MulticastSender sender(multicast_ip, port);
    std::cout << "loaded" << std::endl;
    std::thread hb_thread([&](){
        std::cout << "thread created" << std::endl;
    Heartbeat heartbeat;

    while (true) {
        heartbeat.update_timestamp();
        std::vector<uint8_t> heartbeatMessage(heartbeat.size());
        heartbeat.serialize(heartbeatMessage.data());
        sender.sendMulticast(heartbeatMessage);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    });

    hb_thread.join();
    return 0;
}
