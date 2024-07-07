#include "heartbeat.h"


using namespace std;

Heartbeat::Heartbeat(int64_t timestamp_ms, int32_t timestamp_ns, uint64_t life_counter, uint8_t error_code,uint8_t status){
    this->timestamp_ms = timestamp_ms;
    this->timestamp_ns = timestamp_ns;
    this->life_counter = life_counter;
    this->error_code = error_code;
    this->status = status;

}

Heartbeat::Heartbeat() : timestamp_ms(0), timestamp_ns(0), life_counter(0), error_code(0), status(0) {}

void Heartbeat::update_life_counter()
{
    this->life_counter +=1;
}

void Heartbeat::serialize(uint8_t* buffer) const{
    memcpy(buffer, this, size());
}

Heartbeat Heartbeat::deserialize(const uint8_t* buffer){
    Heartbeat hb;
    memcpy(&hb,buffer,size());
    return hb;
}

void Heartbeat::update_timestamp(){
    auto now = chrono::system_clock::now();
    auto epoch = now.time_since_epoch();
    timestamp_ms = chrono::duration_cast<chrono::milliseconds>(epoch).count();
    timestamp_ns = chrono::duration_cast<chrono::milliseconds>(epoch).count()%1000000;
}
