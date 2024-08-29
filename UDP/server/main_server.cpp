//< server main app.
// purpose of this main server component is to validate HB, send commands via GUI and keep track of current status of client.
#pragma once
#include "multicast_receiver.h"
#include "msg/heartbeat.h"
#include "utils/utils.h"
#include <chrono>
#include <cctype>
#include <thread>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <iostream>
#include "utils/logger.h"

enum HeartbeatValidationError {
    HB_ERROR_NONE           = 0b00000000, // No error
    HB_ERROR_CODE           = 0b00000001, // Error code is non-zero
    HB_ERROR_STATUS         = 0b00000010, // Status is not OK
    HB_ERROR_TIMESTAMP      = 0b00000100, // Timestamp is invalid
    HB_ERROR_LIFE_COUNTER   = 0b00001000,  // Life counter is invalid
    HB_CHECK_SUM_ERROR      = 0b00010000  // checksum invalid
};
uint8_t hb_error_flags = HB_ERROR_NONE;


uint32_t previous_life_counter = 0;
uint8_t tolerance = 100; // in ms
uint64_t timestamp_ms_previous=static_cast<uint64_t>( std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
Logger logger("/home/lukas-kozel/raspberry-pi-playground/UDP/server/Configuration/config.conf");

bool validate_hb_msg(Heartbeat *hb);
bool validate_timestamp(uint64_t timestamp_ms_current, uint64_t tolerance);
bool validate_life_counter(uint32_t life_counter);

int main(int argc, char* argv[]) {
    ArgParser argParser;
    parse_arguments(argc,argv,argParser);

    const std::string multicast_ip = "224.0.0.1";
    const int port = 8081;
    std::unique_ptr<MulticastReceiver> receiver = std::make_unique<MulticastReceiver>(multicast_ip, port);
    std::vector<uint8_t> heartbeatMessage(1024);
    Heartbeat hb_msg;
    if(argParser.debug) std::cout << "loaded" << std::endl;
    std::thread hb_thread([&](){    
        if(argParser.debug) std::cout << "thread created" << std::endl;
        while (true){

            receiver->receive(heartbeatMessage);

            if (!heartbeatMessage.empty()) {
                hb_msg.deserialize(heartbeatMessage.data());
                if(!validate_hb_msg(&hb_msg)){
                    std::cerr << "Heartbeat message is not valid" << std::endl;
                    logger.log(ERROR,"Heartbeat message is not valid");
                    //exit(EXIT_FAILURE);
                } 
                previous_life_counter = hb_msg.life_counter;
                timestamp_ms_previous = hb_msg.timestamp_ms;
                std::ostringstream oss;
                oss << "time_ms = " << hb_msg.timestamp_ms 
                        << " ; time_ns = " << hb_msg.timestamp_ns 
                        << " ; life_counter = " << hb_msg.life_counter
                        << " ; error_code = " << (int)hb_msg.error_code 
                        << " ; status = " << (int)hb_msg.status; 
                logger.log(DEBUG,oss.str()+"\n");                 
                if(argParser.debug) std::cout << oss.str() << std::endl;
            } else {
                logger.log(DEBUG,"No new data received\n"); 
                if(argParser.debug) std::cout << "No new data received\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
    });

    hb_thread.join();
    return 0;
}

bool validate_hb_msg(Heartbeat *hb){ //TODO: refactor this to version via binary ok steps to determine, which of those is not ok. 
    //bool checksum_ok = checksum(sizeof(hb),heartbeatMessage);
    bool error_code_ok = hb->error_code ==0;
    bool status_ok = hb->status == OK_STATUS;
    bool timestamp_ok = validate_timestamp(hb->timestamp_ms, tolerance);
    bool life_counter_ok = validate_life_counter(hb->life_counter);
    if(!error_code_ok){
        hb_error_flags |= HB_ERROR_CODE;
        return false;
    }
    if(!status_ok){
        hb_error_flags |= HB_ERROR_STATUS;
        return false;
    }
    if(!timestamp_ok){
        hb_error_flags |= HB_ERROR_TIMESTAMP;
        return false;
    }
    if(!life_counter_ok){
        hb_error_flags |= HB_ERROR_LIFE_COUNTER;
        return false;
    }
    return false;
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

// bool checksum(size_t received_size,size_t expected_size){
//     return (received_size == expected_size);
// }

void check_and_report_failure(){
    if(hb_error_flags != HB_ERROR_CODE){
        if(hb_error_flags & HB_ERROR_CODE){
            logger.log(ERROR,"HB failure, incomming error code is not 0");
        }
        if(hb_error_flags & HB_ERROR_STATUS){
            logger.log(ERROR,"HB failure, incomming status is not 0");
        }
        if(hb_error_flags & HB_ERROR_LIFE_COUNTER){
            logger.log(ERROR,"HB failure, incomming life counter is not valid");
        }
        if(hb_error_flags & HB_ERROR_TIMESTAMP){
            logger.log(ERROR,"HB failure, incomming timestamp is not valid");
        }
    }
}

