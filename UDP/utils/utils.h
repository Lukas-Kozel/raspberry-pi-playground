#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <cstring>
#include <unistd.h>

struct ArgParser {
    bool debug; //flag in struct to be able to use different configuration for different components via one .h file
    ArgParser(): debug(false){} //constructor to have always debug defined
};
void parse_arguments(int argc, char* argv[], ArgParser& argParser);

#endif //UTILS_H