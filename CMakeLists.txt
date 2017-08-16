cmake_minimum_required(VERSION 3.7)
project(Webe)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/utils.h src/server.cpp
        src/server.h src/client.cpp
        src/client.h src/http_response.cpp
        src/http_response.h src/http_request.cpp
        src/http_request.h src/read_buffer.cpp
        src/read_buffer.h)

add_library(module_404 SHARED src/modules/module_404.cpp src/modules/module_base.h)
add_library(module_info SHARED src/modules/module_info.cpp src/modules/module_base.h)
add_library(module_file_server SHARED src/modules/module_file_server.cpp src/modules/module_base.h)

add_executable(webed src/main.cpp ${SOURCE_FILES})
target_link_libraries(webed dl module_404 module_info module_file_server)
add_executable(webectl src/webectl.cpp ${SOURCE_FILES})
target_link_libraries(webectl dl module_404 module_info module_file_server)
