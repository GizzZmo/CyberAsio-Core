cmake_minimum_required(VERSION 3.16)
project(CyberAsioCore VERSION 1.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Threads REQUIRED)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/webserver.cpp
    src/audio_engine.cpp
    src/device_manager.cpp
    src/config_manager.cpp
)

# Create executable
add_executable(cyberasio-core ${SOURCES})

# Link libraries
target_link_libraries(cyberasio-core 
    PRIVATE 
    Threads::Threads
)

# Copy static files to build directory
add_custom_target(copy_static_files ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/static
    ${CMAKE_CURRENT_BINARY_DIR}/static
    COMMENT "Copying static files"
)

# Platform specific configurations
if(WIN32)
    target_link_libraries(cyberasio-core PRIVATE ws2_32 wsock32)
    target_compile_definitions(cyberasio-core PRIVATE _WIN32_WINNT=0x0601)
endif()

# Compiler specific options
if(MSVC)
    target_compile_options(cyberasio-core PRIVATE /W4)
else()
    target_compile_options(cyberasio-core PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Debug configuration
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(cyberasio-core PRIVATE DEBUG=1)
endif()