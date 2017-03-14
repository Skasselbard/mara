cmake_minimum_required(VERSION 3.3)
project(mara CXX)

set(CMAKE_CPP_FLAGS "${CMAKE_CPP_FLAGS}")

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
add_executable(mara_test main.cpp)
TARGET_LINK_LIBRARIES(mara_test mara)
