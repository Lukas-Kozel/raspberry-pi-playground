//< server main app.
// purpose of this main server component is to validate HB, send commands via GUI and keep track of current status of client.
#include "main_server.h"

static void UDPServer::init(int argc, char* argv[]){
    argParser = std::make_unique<ArgParser>();
    parse_arguments(argc,argv,argParser.get());
    hb_error_flags = HB_ERROR_NONE;
    receiver = std::make_unique<MulticastReceiver>(MULTICAST_IP,PORT);
    receiver->registerCallback(&UDPServer::process_msg_callback);
    previous_life_counter = 0;
    tolerance = 100; // in ms
    timestamp_ms_previous=static_cast<uint64_t>( std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    logger = std::make_unique<Logger>("/home/lukas-kozel/raspberry-pi-playground/UDP/server/Configuration/config.conf");
    logger->log(INFO, "server component initialized.");
}


int main(int argc, char* argv[]) {
    UDPServer::init(argc,argv); 
    // neccessary only when this main cycle needs to maintain more incomming messages simultanously  
        while (true){
            //std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    return 0;
}

static void UDPServer::process_msg_callback(const std::vector<uint8_t>& heartbeatMessage){
    Heartbeat hb_msg;
    hb_msg.deserialize(heartbeatMessage.data());
                if(!validate_hb_msg(&hb_msg)){
                    std::cerr << "Heartbeat message is not valid" << std::endl;
                    logger->log(ERROR,"Heartbeat message is not valid");
                    //exit(EXIT_FAILURE);
                }else{
                previous_life_counter = hb_msg.life_counter;
                timestamp_ms_previous = hb_msg.timestamp_ms;
                std::ostringstream oss;
                oss << "time_ms = " << hb_msg.timestamp_ms 
                        << " ; time_ns = " << hb_msg.timestamp_ns 
                        << " ; life_counter = " << hb_msg.life_counter
                        << " ; error_code = " << (int)hb_msg.error_code 
                        << " ; status = " << (int)hb_msg.status; 
                logger->log(DEBUG,oss.str()+"\n");                 
                if(argParser.debug) std::cout << oss.str() << std::endl;
                }
}

static bool UDPServer::validate_hb_msg(Heartbeat *hb){ //TODO: refactor this to version via binary ok steps to determine, which of those is not ok. 
    bool checksum_ok = checksum(sizeof(hb),Heartbeat::size());
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
    if(!checksum_ok){
        hb_error_flags |= HB_CHECK_SUM_ERROR;
        return false;
    }
    return true;
}

static bool UDPServer::validate_timestamp(uint64_t timestamp_ms_current, uint64_t tolerance) {
    if ((timestamp_ms_current >= timestamp_ms_previous) && (timestamp_ms_current - timestamp_ms_previous <= tolerance)) {
        return true;
    }
    return false;
}

static bool UDPServer::validate_life_counter(uint32_t life_counter){
    if (life_counter > previous_life_counter) {
        return true;
    }
    return false;
}

static bool UDPServer::checksum(size_t received_size,size_t expected_size){
    return (received_size == expected_size);
}

static void UDPServer::check_and_report_failure(){
    if(hb_error_flags != HB_ERROR_CODE){
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
}