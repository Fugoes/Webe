cmake_minimum_required(VERSION 3.7)
project(Webe)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES src/main.cpp src/utils.h src/server.cpp src/server.h src/client.cpp src/client.h)
add_executable(Webe ${SOURCE_FILES})