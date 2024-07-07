#include "utils.h"


void parse_arguments(int argc, char* argv[],Config& config){
    int opt;
    while((opt = getopt(argc,argv,"d:")) != -1){
        switch(opt){
            case 'd':
                if(std::strcmp(optarg,"true") == 0){
                    config.debug = true;
                    std::cout << "Starting app in debug mode.\n"; 
                }

                else if(std::strcmp(optarg,"false") == 0){
                    config.debug = false;
                    std::cout << "Starting app without debug mode.\n";
                }
                else{
                    std::cerr << "Invalid value for -d. Use 'true' or 'false'.\n";
                    exit(EXIT_FAILURE); // Exit if the value is invalid
                    }
                break;
            default:
                std::cerr << "Usage: " <<argv[0] << "[-d true|false] \n";
                exit(EXIT_FAILURE); // Exit if the value is invalid
        }
    }
    if(optind == 1) std::cout << "starting without debug mode \n";
}