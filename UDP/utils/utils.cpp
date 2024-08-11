#include "utils.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cstdlib>

void parse_arguments(int argc, char* argv[], ArgParser& argParser) {
    int opt;
    while ((opt = getopt(argc, argv, "d:h")) != -1) {
        switch (opt) {
            case 'd':
                if (optarg != nullptr) {
                    if (std::strcmp(optarg, "true") == 0) {
                        argParser.debug = true;
                        std::cout << "Starting app in debug mode.\n";
                    } else if (std::strcmp(optarg, "false") == 0) {
                        argParser.debug = false;
                        std::cout << "Starting app without debug mode.\n";
                    } else {
                        std::cerr << "Invalid value for -d. Use 'true' or 'false'.\n";
                        std::cout << "Usage: " << argv[0] << " [-d true|false] \n";
                        exit(EXIT_FAILURE); // Exit if the value is invalid
                    }
                }
                break;
            case 'h':
                std::cout << "Usage: " << argv[0] << " [-d true|false] \n";
                exit(EXIT_SUCCESS);
            default:
                std::cerr << "Usage: " << argv[0] << " [-d true|false] \n";
                exit(EXIT_FAILURE); // Exit if the value is invalid
        }
    }
    if (optind == 1) {
        std::cout << "Starting without debug mode.\n";
    }
}