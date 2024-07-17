#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <cstring>
#include <unistd.h>

struct Config {
    bool debug; //flag in struct to be able to use different configuration for different components via one .h file
    Config(): debug(false){} //constructor to have always debug defined
};
void parse_arguments(int argc, char* argv[], Config& config);

#endif //UTILS_H