#ifndef HEARTBEAT_H
#define HEARTBEAT_H
#pragma once

#include <cstdint>
#include <chrono>
#include <cstring>
#include <stdio.h>

class Heartbeat{
  public:
    int64_t timestamp_ms; // Milliseconds since Unix epoch
    int32_t timestamp_ns; // Additional nanoseconds
    uint8_t error_code;   // Error code (0-255)
    uint8_t status;       // Status (0-255)

    void serialize(uint8_t* buffer) const; //this will only read data from object and copy it into byte array, that is why it is const
    static Heartbeat deserialize(const uint8_t* buffer);
    void update_timestamp();
    Heartbeat(int64_t timestamp_ms, int32_t timestamp_ns, uint8_t error_code,uint8_t status);
    Heartbeat();
    static constexpr size_t size(){
        return sizeof(timestamp_ms) + sizeof(timestamp_ns) + sizeof(error_code) + sizeof(status);
    };
};

#endif // HEARTBEAT_H