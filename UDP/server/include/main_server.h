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


enum HeartbeatValidationError {
    HB_ERROR_NONE           = 0b00000000, // No error
    HB_ERROR_CODE           = 0b00000001, // Error code is non-zero
    HB_ERROR_STATUS         = 0b00000010, // Status is not OK
    HB_ERROR_TIMESTAMP      = 0b00000100, // Timestamp is invalid
    HB_ERROR_LIFE_COUNTER   = 0b00001000,  // Life counter is invalid
    HB_CHECK_SUM_ERROR      = 0b00010000  // checksum invalid
};

class UDPServer
{
private:

    static const std::string MULTICAST_IP = "224.0.0.1";
    static const int PORT = 8081;

    static uint8_t hb_error_flags;
    static uint32_t previous_life_counter;
    static uint8_t tolerance;
    static uint64_t timestamp_ms_previous;
    static std::unique_ptr<Logger> logger;
    static std::unique_ptr<ArgParser> argParser;
    static std::unique_ptr<MulticastReceiver> receiver;

    static bool validate_hb_msg(Heartbeat* hb);
    static bool validate_timestamp(uint64_t timestamp_ms_current, uint64_t tolerance);
    static bool validate_life_counter(uint32_t life_counter);
    static bool checksum(size_t received_size, size_t expected_size);
    static void check_and_report_failure();
    static void process_msg_callback(const std::vector<uint8_t>& heartbeatMessage);

public:
    static void init(int argc, char* argv[]);
};

#endif // SERVER_MAIN_H
