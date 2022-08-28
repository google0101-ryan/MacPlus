project(MacPlus)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

set(CMAKE_CXX_STANDARD 23)

set(SOURCES src/main.cpp
            src/Bus.cpp
            src/CPU.cpp)

set(CMAKE_BUILD_TYPE DEBUG)

cmake_minimum_required(VERSION 3.20)

add_executable(MacPlus ${SOURCES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)