//< raspberry Pi main app.
// purpose of this main component is to recieve data from other client components,
// validate them and send HB, that will contain status or error code of all client components 

#include "Multicast_sender.h"
#include "msg/heartbeat.h"
#include "utils/utils.h"
#include <chrono>
#include <thread>
#include <vector>

int main(int argc, char* argv[]) {
    ArgParser argParser;
    parse_arguments(argc,argv,argParser);
    const std::string multicast_ip = "224.0.0.1";
    const int port = 8081;

    MulticastSender sender(multicast_ip, port);
    if(argParser.debug) std::cout << "loaded" << std::endl;
    std::thread hb_thread([&](){
        if(argParser.debug) std::cout << "thread created" << std::endl;
    Heartbeat heartbeat;

    while (true) {
        heartbeat.update_timestamp();
        heartbeat.update_life_counter();
        std::vector<uint8_t> heartbeatMessage(heartbeat.size());
        heartbeat.serialize(heartbeatMessage.data());
        sender.sendMulticast(heartbeatMessage);
        if(argParser.debug) std::cout << "time_ms = " << heartbeat.timestamp_ms 
        << " ; time_ns = " << heartbeat.timestamp_ns 
        << " ; life_counter = " << heartbeat.life_counter
        << " ; error_code = " << (int)heartbeat.error_code 
        << " ; status = " << (int)heartbeat.status 
        << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    });

    hb_thread.join();
    return 0;
}
