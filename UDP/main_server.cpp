//< server main app.

#include "multicast_receiver.h"
#include "msg/heartbeat.h"
#include "utils.h"
#include <chrono>
#include <cctype>
#include <thread>
#include <vector>
#include <unistd.h>
#include <iostream>

uint32_t previous_life_counter = 0;
uint8_t tolerance = 100; // in ms
uint64_t timestamp_ms_previous=0;

bool validate_hb_msg(Heartbeat *hb);
bool validate_timestamp(uint64_t timestamp_ms_current, uint64_t tolerance);
bool validate_life_counter(uint32_t life_counter);

int main(int argc, char* argv[]) {
    Config config;
    parse_arguments(argc,argv,config);

    const std::string multicast_ip = "224.0.0.1";
    const int port = 8081;
    MulticastReceiver receiver(multicast_ip, port);
    std::vector<uint8_t> heartbeatMessage(1024);

    if(config.debug) std::cout << "loaded" << std::endl;
    std::thread hb_thread([&](){    
        if(config.debug) std::cout << "thread created" << std::endl;
        while (true){

            receiver.receive(heartbeatMessage);

            if (!heartbeatMessage.empty()) {
                Heartbeat hb = Heartbeat::deserialize(heartbeatMessage.data());
                if(!validate_hb_msg(&hb)){
                    std::cerr << "Heartbeat message is not valid" << std::endl;
                    exit(EXIT_FAILURE);
                } 
                previous_life_counter = hb.life_counter;
                timestamp_ms_previous = hb.timestamp_ms;                  
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
    if(hb->error_code != 0 || hb->status != OK_STATUS || !validate_timestamp(hb->timestamp_ms,tolerance) || !validate_life_counter(hb->life_counter)){
        return false;
    }
    return true;
}

bool validate_timestamp(uint64_t timestamp_ms_current, uint64_t tolerance) {
    if ((timestamp_ms_current >= timestamp_ms_previous) && (timestamp_ms_current - timestamp_ms_previous <= tolerance)) {
        return true;
    }
    return false;
}

bool validate_life_counter(uint32_t life_counter){
    if (life_counter > previous_life_counter) {
        return true;
    }
    return false;
}

