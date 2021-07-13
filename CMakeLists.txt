cmake_minimum_required(VERSION 3.13)
project(sdvc)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS "-std=c99 -Wall -Werror -Wno-unused-parameter")

include_directories(src)
file(GLOB SRC_FILES src/*.c)
add_executable(sdvc ${SRC_FILES})
