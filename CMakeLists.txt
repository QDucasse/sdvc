cmake_minimum_required(VERSION 3.13)
project(sdvu)
SET(CMAKE_C_FLAGS "-std=c99 -Wall -Werror -Wno-unused-parameter")

include_directories(src)
file(GLOB SRC_FILES src/*.c)
add_executable(sdvu ${SRC_FILES})
