cmake_minimum_required(VERSION 3.7)
project(Webe)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES src/utils.h src/server.cpp src/server.h src/client.cpp src/client.h src/http_response.cpp src/http_response.h src/http_request.cpp src/http_request.h)
add_executable(Webe src/main.cpp ${SOURCE_FILES})
add_executable(test1 test/test1.cpp ${SOURCE_FILES})
