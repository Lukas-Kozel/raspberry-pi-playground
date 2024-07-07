cmake_minimum_required(VERSION 3.10)

# Set the project name
project(HeartbeatProject)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the source files for the main executable
set(CLIENT_SOURCES
    main_client.cpp
    Multicast_sender.cpp
    client.cpp
    msg/heartbeat.cpp
)

# Add the source files for the main executable
set(SERVER_SOURCES
    main_server.cpp
    multicast_receiver.cpp
    client.cpp
    msg/heartbeat.cpp
)

# Add the source files for the communication tester executable
set(TESTER_SOURCES
    communication_tester.cpp  # Assuming your test file is named test.cpp
    client.cpp
    msg/heartbeat.cpp
)

# Add the main executable target
add_executable(main_client ${CLIENT_SOURCES})

# Add the main executable target
add_executable(main_server ${SERVER_SOURCES})

# Add the communication tester executable target
add_executable(communication_tester ${TESTER_SOURCES})

# Add include directories
target_include_directories(main_client PUBLIC
    "${PROJECT_SOURCE_DIR}/msg"
    "${PROJECT_SOURCE_DIR}"
)

# Add include directories
target_include_directories(main_server PUBLIC
    "${PROJECT_SOURCE_DIR}/msg"
    "${PROJECT_SOURCE_DIR}"
)
target_include_directories(communication_tester PUBLIC
    "${PROJECT_SOURCE_DIR}/msg"
    "${PROJECT_SOURCE_DIR}"
)

# Set the output directory for the binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# Specify the install location for the binaries
install(TARGETS main_client DESTINATION "${PROJECT_SOURCE_DIR}/bin")
install(TARGETS main_server DESTINATION "${PROJECT_SOURCE_DIR}/bin")
install(TARGETS communication_tester DESTINATION "${PROJECT_SOURCE_DIR}/bin")
