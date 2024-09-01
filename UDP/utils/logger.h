#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <string>
#include<ctype.h>
#include <fstream>
#include "include/inih/cpp/INIReader.h"
#include <filesystem>
#include <unordered_map>


enum LOG_LEVEL{
ERROR,
 DEBUG,
  WARNING,
   INFO
};
extern std::unordered_map<LOG_LEVEL, std::string> LOG_LEVEL_MAP;

struct Config{
std::string file;// = "./logs/error_log.txt"
LOG_LEVEL log_level;
uint8_t max_size; // in MB
uint8_t max_files;
};

class Logger
{

private:
    std::string log_file_location;
    std::ofstream main_log_file;
    std::ofstream error_log_file;
    Config main_log_config;
    Config error_log_config;
    uint8_t number_of_main_log_files=0;
    uint8_t number_of_error_log_files=0;
    void write_log(LOG_LEVEL log_level, const std::string& message);
    LOG_LEVEL parse_log_level(const std::string& level);
    std::ofstream open_file_for_log(std::string& log_file);
    void close_file(std::ofstream &log_file);
    //returns size in MegaBytes
    std::uintmax_t get_file_size(const std::string& file_path); 
    void rename_file(const std::string& file_path);
    std::string insert_before_extension(const std::string &file_path, int index);
    void perform_operation_log_files();
    std::string get_current_timestamp();
    std::string get_log_level_string(LOG_LEVEL log_level);

public:
    Logger(const std::string& config_path); //only for initialization of logger. 
    ~Logger();
    void log(LOG_LEVEL log_level, const std::string& message);
};

#endif 