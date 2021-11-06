cmake_minimum_required(VERSION 3.16)

project(usb2_emulation
    VERSION 0.0.1
    LANGUAGES CXX
    )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

file(GLOB SOURCES src/*.cpp)
add_executable(usb2_emulation ${SOURCES})

target_include_directories(usb2_emulation PRIVATE include/)

