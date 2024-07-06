cmake_minimum_required(VERSION 3.10)

# Set the project name
project(HeartbeatProject)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the source files
set(SOURCES
    main.cpp
    Multicast_sender.cpp
    client.cpp
    msg/heartbeat.cpp
    #server.cpp
)

# Add an executable target
add_executable(main ${SOURCES})

# Add include directories
target_include_directories(main PUBLIC
    "${PROJECT_SOURCE_DIR}/msg"
    "${PROJECT_SOURCE_DIR}"
)

# Set the output directory for the binary
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# Specify the install location for the binary
install(TARGETS main DESTINATION "${PROJECT_SOURCE_DIR}/bin")
