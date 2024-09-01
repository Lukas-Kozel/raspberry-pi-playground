#ifndef SERVER_MAIN_H
#define SERVER_MAIN_H

#include "multicast_receiver.h"
#include "msg/heartbeat.h"
#include "utils/utils.h"
#include <chrono>
#include <cctype>
#include <thread>
#include <string>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <iostream>
#include "utils/logger.h"
#include <bitset>


enum HeartbeatValidationError {
    HB_ERROR_NONE           = 0b00000000, // No error
    HB_ERROR_CODE           = 0b00000001, // Error code is non-zero
    HB_ERROR_STATUS         = 0b00000010, // Status is not OK
    HB_ERROR_TIMESTAMP      = 0b00000100, // Timestamp is invalid
    HB_ERROR_LIFE_COUNTER   = 0b00001000,  // Life counter is invalid
    HB_CHECK_SUM_ERROR      = 0b00010000  // checksum invalid
};

const std::string MULTICAST_IP = "224.0.0.1";
const int PORT = 8081;
const std::string LOGGER_CONFIG_PATH = "/home/luky/playground/raspberry_pi/UDP/server/Configuration/config.conf";


class UDPServer
{
private:

     uint8_t hb_error_flags;
     uint32_t previous_life_counter;
     uint8_t tolerance;
     uint64_t timestamp_ms_previous;
     std::unique_ptr<Logger> logger;
     std::unique_ptr<ArgParser> argParser;
     std::unique_ptr<MulticastReceiver> receiver;

     bool validate_hb_msg(const Heartbeat &hb);
     bool validate_timestamp(uint64_t timestamp_ms_current, uint64_t tolerance);
     bool validate_life_counter(uint32_t life_counter);
     bool checksum(size_t received_size, size_t expected_size);
     void check_and_report_failure();
     void process_msg_callback(const std::vector<uint8_t>& heartbeatMessage);

public:
    UDPServer(const std::string& multicast_ip, int port, const std::string& logger_config_path);
    void init(int argc, char* argv[]);
    void run();

};

#endif // SERVER_MAIN_H
