//< server main app.

#include "multicast_receiver.h"
#include "msg/heartbeat.h"
#include "utils.h"
#include <chrono>
#include <thread>
#include <vector>
#include <unistd.h>
#include <iostream>


bool validate_hb_msg(Heartbeat *hb);

int main(int argc, char* argv[]) {
    Config config;
    parse_arguments(argc,argv,config);

    const std::string multicast_ip = "224.0.0.1";
    const int port = 8081;
    MulticastReceiver receiver(multicast_ip, port);
    std::vector<uint8_t> heartbeatMessage(1024);
    uint32_t expected_life_counter = 0;
    uint64_t expected_timestamp_ms = 0;
    if(config.debug) std::cout << "loaded" << std::endl;
    std::thread hb_thread([&](){    
        if(config.debug) std::cout << "thread created" << std::endl;
        while (true){

            receiver.receive(heartbeatMessage);

            if (!heartbeatMessage.empty()) {
                Heartbeat hb = Heartbeat::deserialize(heartbeatMessage.data());
                if(config.debug) std::cout << "time_ms = " << hb.timestamp_ms 
                        << " ; time_ns = " << hb.timestamp_ns 
                        << " ; life_counter = " << hb.life_counter
                        << " ; error_code = " << (int)hb.error_code 
                        << " ; status = " << (int)hb.status 
                        << std::endl;
            } else {
                if(config.debug) std::cout << "No new data received\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
    });

    hb_thread.join();
    return 0;
}

bool validate_hb_msg(Heartbeat *hb){
}

