#include "msg/heartbeat.h"
#include "client.h"
#include <chrono>
#include <thread>
#include <vector>
#include <iostream>

int main() {
    const std::string multicast_ip = "224.0.0.1";
    const int port = 8081;
    std::cout << "ip and port succesfull";
    UDPClient client(multicast_ip, port);
    std::vector<uint8_t> heartbeatMessage_prev;
    std::cout << "initializoation was succesfull";

    while (true) {
        std::vector<uint8_t> heartbeatMessage(1024);

        client.receive(heartbeatMessage);

        if (!heartbeatMessage.empty() && heartbeatMessage != heartbeatMessage_prev) {
            Heartbeat hb = Heartbeat::deserialize(heartbeatMessage.data());
            std::cout << "time_ms = " << hb.timestamp_ms 
                      << " ; time_ns = " << hb.timestamp_ns 
                      << " ; error_code = " << (int)hb.error_code 
                      << " ; status = " << (int)hb.status 
                      << std::endl;

            heartbeatMessage_prev = heartbeatMessage;
        } else {
            std::cout << "No new data received\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    return 0;
}
