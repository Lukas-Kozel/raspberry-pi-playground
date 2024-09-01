#ifndef HEARTBEAT_H
#define HEARTBEAT_H
#pragma once

#include <cstdint>
#include <chrono>
#include <cstring>
#include <stdio.h>



enum Status{
  OK_STATUS,
  ERROR_STATUS,
  TIMEOUT_STATUS
};

class Heartbeat{
  public:
    uint64_t timestamp_ms; // Milliseconds since Unix epoch
    uint32_t timestamp_ns; // Additional nanoseconds
    uint64_t life_counter;
    uint8_t error_code;   // Error code (0-255)
    Status status;       // Status (0-2)

    void serialize(uint8_t* buffer) const; //this will only read data from object and copy it into byte array, that is why it is const
    static Heartbeat deserialize(const uint8_t* buffer);
    void update_timestamp();
    Heartbeat(uint64_t timestamp_ms, uint32_t timestamp_ns, uint64_t life_counter, uint8_t error_code,Status status );
    Heartbeat();
    void update_life_counter();
    static constexpr size_t size(){
      return sizeof(Heartbeat);
        //return sizeof(timestamp_ms) + sizeof(timestamp_ns)+ sizeof(life_counter) + sizeof(error_code) + sizeof(status);
    };
};
#endif // HEARTBEAT_H