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

add_executable(webed src/main.cpp ${SOURCE_FILES})
target_link_libraries(webed dl module_404 module_info)
add_executable(webectl src/webectl.cpp src/utils.h)
add_executable(test1 test/test1.cpp ${SOURCE_FILES})
target_link_libraries(test1 dl)
add_executable(test2 test/test2.cpp ${SOURCE_FILES})
target_link_libraries(test2 dl)
add_executable(test3 test/test3.cpp ${SOURCE_FILES})
target_link_libraries(test3 dl)
add_executable(test4 test/test4.cpp ${SOURCE_FILES})
target_link_libraries(test4 dl)
