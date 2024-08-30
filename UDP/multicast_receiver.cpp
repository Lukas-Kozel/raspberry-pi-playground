#include "multicast_receiver.h"

void MulticastReceiver::receiveMulticast(std::vector<uint8_t> &message)
{
    receive(message);
    if (callback && !message.empty()) { //msg is not empty and callback is set.
        callback(message);
    }
    else if(message.empty()){
        //TODO log
    }
}

MulticastReceiver::MulticastReceiver(const std::string& multicast_ip, int port): UDPClient(multicast_ip,port){};

MulticastReceiver::~MulticastReceiver() {
    if (listening_thread.joinable()) {
        listening_thread.join(); // Clean up the thread
    }
}

void MulticastReceiver::registerCallback(std::function<void(const std::vector<uint8_t> &)> callback)
{
    this->callback = callback;
    this->listening_thread = std::thread(&MulticastReceiver::listen_for_message,this);
}

void MulticastReceiver::listen_for_message()
{
    std::vector<uint8_t> buffer(1024);
    while(true){
        receive(buffer);
        if(!buffer.empty() && this->callback){
            this->callback(buffer);
        }
    }
}
