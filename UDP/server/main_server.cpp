//< server main app.
// purpose of this main server component is to validate HB, send commands via GUI and keep track of current status of client.

#include "multicast_receiver.h"
#include "msg/heartbeat.h"
#include "utils/utils.h"
#include <chrono>
#include <cctype>
#include <thread>
#include <vector>
#include <unistd.h>
#include <iostream>
#include "utils/logger.h"

uint32_t previous_life_counter = 0;
uint8_t tolerance = 100; // in ms
uint64_t timestamp_ms_previous=static_cast<uint64_t>( std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());

bool validate_hb_msg(Heartbeat *hb);
bool validate_timestamp(uint64_t timestamp_ms_current, uint64_t tolerance);
bool validate_life_counter(uint32_t life_counter);

int main(int argc, char* argv[]) {
    ArgParser argParser;
    parse_arguments(argc,argv,argParser);

    const std::string multicast_ip = "224.0.0.1";
    const int port = 8081;
    MulticastReceiver receiver(multicast_ip, port);
    std::vector<uint8_t> heartbeatMessage(1024);

    if(argParser.debug) std::cout << "loaded" << std::endl;
    std::thread hb_thread([&](){    
        if(argParser.debug) std::cout << "thread created" << std::endl;
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
                if(argParser.debug) std::cout << "time_ms = " << hb.timestamp_ms 
                        << " ; time_ns = " << hb.timestamp_ns 
                        << " ; life_counter = " << hb.life_counter
                        << " ; error_code = " << (int)hb.error_code 
                        << " ; status = " << (int)hb.status 
                        << std::endl;
            } else {
                if(argParser.debug) std::cout << "No new data received\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
    });

    hb_thread.join();
    return 0;
}

bool validate_hb_msg(Heartbeat *hb){ //TODO: refactor this to version via binary ok steps to determine, which of those is not ok. 
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

