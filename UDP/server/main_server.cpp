//< server main app.
// purpose of this main server component is to validate HB, send commands via GUI and keep track of current status of client.
#include "main_server.h"

UDPServer::UDPServer(const std::string& multicast_ip, int port, const std::string& logger_config_path) {
    receiver = std::make_unique<MulticastReceiver>(multicast_ip, port);
    logger = std::make_unique<Logger>(logger_config_path);
}

void UDPServer::init(int argc, char* argv[]) {
    timestamp_ms_previous = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::system_clock::now().time_since_epoch()).count());
    previous_life_counter = 0;
    tolerance = 200; // in ms
    hb_error_flags = HB_ERROR_NONE;
    argParser = std::make_unique<ArgParser>();
    parse_arguments(argc, argv, *argParser.get());
    receiver->registerCallback(std::bind(&UDPServer::process_msg_callback,this,std::placeholders::_1));
    logger->log(INFO, "Server component initialized.");
}

void UDPServer::run()
{
    logger->log(DEBUG, "Server's main loop started.");
    // neccessary only when this main cycle needs to maintain more incomming messages simultanously  
        while (true){
            //std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
}

void UDPServer::process_msg_callback(const std::vector<uint8_t>& heartbeatMessage) {
    hb_error_flags = HB_ERROR_NONE;
    Heartbeat hb_msg;
    hb_msg = hb_msg.deserialize(heartbeatMessage.data());

    if (!validate_hb_msg(hb_msg)) {
        logger->log(ERROR, "Heartbeat message is not valid");
        check_and_report_failure();
            std::ostringstream oss;
            oss << "time_ms = " << hb_msg.timestamp_ms
                << " ; time_ns = " << hb_msg.timestamp_ns
                << " ; life_counter = " << hb_msg.life_counter
                << " ; error_code = " << static_cast<int>(hb_msg.error_code)
                << " ; status = " << static_cast<int>(hb_msg.status);
            logger->log(DEBUG, oss.str());
        previous_life_counter = hb_msg.life_counter;
        timestamp_ms_previous = hb_msg.timestamp_ms;
        return;
    }

    previous_life_counter = hb_msg.life_counter;
    timestamp_ms_previous = hb_msg.timestamp_ms;
    std::ostringstream oss;
    oss << "time_ms = " << hb_msg.timestamp_ms
        << " ; time_ns = " << hb_msg.timestamp_ns
        << " ; life_counter = " << hb_msg.life_counter
        << " ; error_code = " << static_cast<int>(hb_msg.error_code)
        << " ; status = " << static_cast<int>(hb_msg.status);
    logger->log(INFO, oss.str());
}

bool UDPServer::validate_hb_msg(const Heartbeat &hb){ //TODO: refactor this to version via binary ok steps to determine, which of those is not ok. 
    bool checksum_ok = checksum(sizeof(hb),Heartbeat::size());
    bool error_code_ok = (hb.error_code == 0);
    bool status_ok = (hb.status == OK_STATUS);
    bool timestamp_ok = validate_timestamp(hb.timestamp_ms, tolerance);
    bool life_counter_ok = validate_life_counter(hb.life_counter);
    if(!error_code_ok){
        hb_error_flags |= HB_ERROR_CODE;
    }
    if(!status_ok){
        hb_error_flags |= HB_ERROR_STATUS;
    }
    if(!timestamp_ok){
        hb_error_flags |= HB_ERROR_TIMESTAMP;
    }
    if(!life_counter_ok){
        hb_error_flags |= HB_ERROR_LIFE_COUNTER;
    }
    if(!checksum_ok){
        hb_error_flags |= HB_CHECK_SUM_ERROR;
    }
    return (hb_error_flags == HB_ERROR_NONE);
}

bool UDPServer::validate_timestamp(uint64_t timestamp_ms_current, uint64_t tolerance) {
    if ((timestamp_ms_current >= timestamp_ms_previous) && (timestamp_ms_current - timestamp_ms_previous <= tolerance)) {
        return true;
    }

    std::ostringstream oss;
    oss << "received ms = " << timestamp_ms_current 
        << "; expected ms = " << timestamp_ms_previous 
        << ", diff = " << (timestamp_ms_current - timestamp_ms_previous)
        << "\n";
    logger->log(DEBUG, oss.str());

    return false;
}


bool UDPServer::validate_life_counter(uint32_t life_counter){
    if (life_counter > previous_life_counter) {
        return true;
    }
    std::ostringstream oss;
    oss << "received life counter = " << life_counter 
        << "; expected life counter = " << previous_life_counter+1;
    logger->log(DEBUG, oss.str());
    return false;
}

bool UDPServer::checksum(size_t received_size,size_t expected_size){
        std::ostringstream oss;
    oss << "received size = " << received_size 
        << "; expected size = " << expected_size;
    logger->log(DEBUG, oss.str());
    return (received_size == expected_size);
}

void UDPServer::check_and_report_failure(){
        std::bitset<8> binary_flags(hb_error_flags);

        std::ostringstream oss;
        oss << "HB error flags (binary): 0b" << binary_flags;
        logger->log(ERROR, oss.str());
        if(hb_error_flags & HB_ERROR_CODE){
            logger->log(ERROR,"HB failure, incomming error code is not 0");
        }
        if(hb_error_flags & HB_ERROR_STATUS){
            logger->log(ERROR,"HB failure, incomming status is not 0");
        }
        if(hb_error_flags & HB_ERROR_LIFE_COUNTER){
            logger->log(ERROR,"HB failure, incomming life counter is not valid");
        }
        if(hb_error_flags & HB_ERROR_TIMESTAMP){
            logger->log(ERROR,"HB failure, incomming timestamp is not valid");
        }
}


int main(int argc, char* argv[]) {
    UDPServer server(MULTICAST_IP,PORT,LOGGER_CONFIG_PATH);
    server.init(argc,argv); 
    server.run();
    return 0;
}