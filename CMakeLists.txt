cmake_minimum_required(VERSION 3.5)
project(mara)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(HEADER_FILES
    include/interface.h
        include/PageList.h
        include/Page.h
        include/Logger.h
        )

set(SOURCE_FILES
        ${HEADER_FILES}
        main.cpp
        src/interface.cpp
        src/PageList.cpp
        src/Page.cpp
        src/Logger.cpp
        )

set(CMAKE_INCLUDE_PATH
        ${CMAKE_INCLUDE_PATH}
        include
        )
add_executable(mara ${SOURCE_FILES})