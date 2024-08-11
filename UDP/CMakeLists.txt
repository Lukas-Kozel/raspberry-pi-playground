cmake_minimum_required(VERSION 3.10)

# Set the project name
project(HeartbeatProject)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set include libs path
set(INCLUDE_INIH /home/luky/playground/raspberry_pi/UDP/utils/include/inih/cpp)
set(INCLUDE_INI_C /home/luky/playground/raspberry_pi/UDP/utils/include/inih)

# Check if a toolchain file was specified
if(DEFINED CMAKE_TOOLCHAIN_FILE)
    message(STATUS "Toolchain file specified: ${CMAKE_TOOLCHAIN_FILE}")
    set(IS_CROSS_COMPILING TRUE)
else()
    set(IS_CROSS_COMPILING FALSE)
endif()

if(IS_CROSS_COMPILING)
    # Cross-compilation settings for the Raspberry Pi
    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_SYSTEM_PROCESSOR aarch64)
    set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
    set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)
    
    # Add the source files for the main executable
    set(CLIENT_SOURCES
        client/main_client.cpp
        Multicast_sender.cpp
        client.cpp
        msg/heartbeat.cpp
        utils/utils.cpp
        utils/logger.cpp
        ${INCLUDE_INIH}/INIReader.cpp
        ${INCLUDE_INI_C}/ini.c
    )

    # Add the main executable target
    add_executable(main_client ${CLIENT_SOURCES})
    target_include_directories(main_client PUBLIC
        "${PROJECT_SOURCE_DIR}/msg"
        "${PROJECT_SOURCE_DIR}"
        ${INCLUDE_INIH}
    )

    install(TARGETS main_client DESTINATION "${PROJECT_SOURCE_DIR}/bin/rpi")

else()
    # Add the source files for the main executable
    set(CLIENT_SOURCES
        client/main_client.cpp
        Multicast_sender.cpp
        client.cpp
        msg/heartbeat.cpp
        utils/utils.cpp
        utils/logger.cpp
        ${INCLUDE_INIH}/INIReader.cpp
        ${INCLUDE_INI_C}/ini.c
    )
    # Add the source files for the server executable
    set(SERVER_SOURCES
        server/main_server.cpp
        multicast_receiver.cpp
        client.cpp
        msg/heartbeat.cpp
        utils/utils.cpp
        utils/logger.cpp
        ${INCLUDE_INIH}/INIReader.cpp
        ${INCLUDE_INI_C}/ini.c
    )

    # Add the source files for the communication tester executable
    set(TESTER_SOURCES
        communication_tester.cpp  # Assuming your test file is named test.cpp
        client.cpp
        msg/heartbeat.cpp
    )

    # Add the main executable target
    add_executable(main_client ${CLIENT_SOURCES})
    add_executable(main_server ${SERVER_SOURCES})
    add_executable(communication_tester ${TESTER_SOURCES})

    # Add include directories
    target_include_directories(main_client PUBLIC
        "${PROJECT_SOURCE_DIR}/msg"
        "${PROJECT_SOURCE_DIR}"
        ${INCLUDE_INIH}
    )

    target_include_directories(main_server PUBLIC
        "${PROJECT_SOURCE_DIR}/msg"
        "${PROJECT_SOURCE_DIR}"
        ${INCLUDE_INIH}
    )

    target_include_directories(communication_tester PUBLIC
        "${PROJECT_SOURCE_DIR}/msg"
        "${PROJECT_SOURCE_DIR}"
    )

    install(TARGETS main_client DESTINATION "${PROJECT_SOURCE_DIR}/bin/pc")
    install(TARGETS main_server DESTINATION "${PROJECT_SOURCE_DIR}/bin/pc")
    install(TARGETS communication_tester DESTINATION "${PROJECT_SOURCE_DIR}/bin/pc")
endif()
