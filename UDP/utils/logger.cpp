#include "logger.h"

void Logger::write_log(LOG_LEVEL log_level, const std::string& message) {
    perform_operation_log_files();
    if(log_level == LOG_LEVEL::ERROR){
        main_log_file << "[" << log_level << "]" << message << std::endl;
        error_log_file << "[" << log_level << "]" << message << std::endl;
    }
    else{
        main_log_file << "[" << log_level << "]" << message << std::endl;
    }
}

LOG_LEVEL Logger::parse_log_level(const std::string& level) {
    if (level == "ERROR") return LOG_LEVEL::ERROR;
    if (level == "DEBUG") return LOG_LEVEL::DEBUG;
    if (level == "WARNING") return LOG_LEVEL::WARNING;
    if (level == "INFO") return LOG_LEVEL::INFO;
    return LOG_LEVEL::INFO; // Default
}

std::ofstream Logger::open_file_for_log(std::string &log_file)
{
    std::ofstream file(log_file, std::ios_base::app);
    if(file.is_open()){
        return file;
    }
    else{
        std::cerr << "Cannot open log file" << std::endl;
        exit(EXIT_FAILURE);
    }
}

Logger::Logger(const std::string &config_path)
{
    INIReader reader(config_path);
    if(reader.ParseError() < 0){
        std::cerr << "Cannot load config file" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string section = "main_log";
    main_log_config.file = reader.GetString(section,"file","error");
    if(main_log_config.file == "error"){
        std::cerr << "Cannot find main_log:file" << std::endl;
        exit(EXIT_FAILURE);
    }
    main_log_config.log_level = parse_log_level(reader.GetString(section,"log_level","INFO"));
    main_log_config.max_size = reader.GetInteger("main_log", "max_size", -1);
    if(main_log_config.max_size  < 0){
        std::cerr << "Cannot find main_log:max_size" << std::endl;
        exit(EXIT_FAILURE);
    }
    main_log_config.max_files = reader.GetInteger("main_log", "max_files", -1);
        if(main_log_config.max_files  < 0){
        std::cerr << "Cannot find main_log:max_files" << std::endl;
        exit(EXIT_FAILURE);
    }
    main_log_file = open_file_for_log(main_log_config.file);

    section = "error_log";
    error_log_config.file = reader.GetString(section,"file","error");
    if(error_log_config.file == "error"){
        std::cerr << "Cannot find main_log:file" << std::endl;
        exit(EXIT_FAILURE);
    }
    error_log_config.log_level = parse_log_level(reader.GetString(section,"log_level","ERROR"));
    error_log_config.max_size = reader.GetInteger("main_log", "max_size", -1);
    if(error_log_config.max_size  < 0){
        std::cerr << "Cannot find main_log:max_size" << std::endl;
        exit(EXIT_FAILURE);
    }
    error_log_config.max_files = reader.GetInteger("main_log", "max_files", -1);
        if(error_log_config.max_files  < 0){
        std::cerr << "Cannot find main_log:max_files" << std::endl;
        exit(EXIT_FAILURE);
    }
    error_log_file = open_file_for_log(error_log_config.file);

}

void Logger::log(LOG_LEVEL log_level, const std::string &message)
{
    write_log(log_level,message);
}

void Logger::close_file(std::ofstream& log_file) {
    if (log_file.is_open()) {
        log_file.close();
    }
}

std::uintmax_t Logger::get_file_size(const std::string &file_path)
{
    std::filesystem::path path(file_path);
    if(std::filesystem::exists(path)){ return std::filesystem::file_size(path)/1048576;} //convertion from bytes to MB
    else {
        std::cerr << "File does not exist " << file_path << std::endl;
        return 0;
    }
}

void Logger::rename_file(const std::string &file_path)
{
    std::filesystem::path old_path(file_path);
    std::filesystem::path new_path;
    if(old_path == main_log_config.file) new_path = std::filesystem::path(insert_before_extension(file_path, number_of_main_log_files++));
    if((old_path == error_log_config.file) == 0) new_path = std::filesystem::path(insert_before_extension(file_path, number_of_error_log_files++));
    
    try
    {
        std::filesystem::rename(old_path,new_path);

    }
    catch(const std::filesystem::filesystem_error& e)
    {
        std::cerr << e.what() << '\n';
    }
    
}

std::string Logger::insert_before_extension(const std::string& file_path, int index){
    std::size_t dot_pos = file_path.find_last_of(".");
    if(dot_pos == std::string::npos){
        //no extension
        return file_path + "_" + std::to_string(index);
    }
    else{
        std::string base = file_path.substr(0,dot_pos);
        std::string extension = file_path.substr(dot_pos);
        return base + "_" + std::to_string(index) + extension;
    }
}

void Logger::perform_operation_log_files()
{
    if(get_file_size(main_log_config.file) >= main_log_config.max_size) {
        if(main_log_config.max_files < number_of_main_log_files){
            //create new file and rename the old one
            rename_file(main_log_config.file);
            main_log_file = open_file_for_log(main_log_config.file);
        }
        else{
            number_of_main_log_files = 0;
            std::filesystem::remove(insert_before_extension(main_log_config.file,number_of_main_log_files++));
            main_log_file = open_file_for_log(main_log_config.file);
        }
    }
        if(get_file_size(error_log_config.file) >= error_log_config.max_size) {
        if(error_log_config.max_files < number_of_main_log_files){
            //create new file and rename the old one
            rename_file(error_log_config.file);
            error_log_file = open_file_for_log(error_log_config.file);
        }
        else{
            number_of_error_log_files = 0;
            std::filesystem::remove(insert_before_extension(error_log_config.file,number_of_error_log_files++));
            error_log_file = open_file_for_log(error_log_config.file);
        }
    }
}

Logger::~Logger() {
    close_file(main_log_file);
    close_file(error_log_file);
}