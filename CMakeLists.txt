cmake_minimum_required(VERSION 3.3)
project(mara CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set(HEADER_FILES
        include/Mara.h
        include/predefined.h
        include/Statistic.h
        )

set(SOURCE_FILES
        ${HEADER_FILES}
        src/Statistic.cpp
        src/Mara.cpp)

set(CMAKE_INCLUDE_PATH
        ${CMAKE_INCLUDE_PATH}
        include
        )

add_library(mara ${SOURCE_FILES})
find_package(Threads)
add_executable(mara_test main.cpp)
target_link_libraries(mara_test ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(mara_test mara)
