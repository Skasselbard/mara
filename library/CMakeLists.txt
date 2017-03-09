cmake_minimum_required(VERSION 3.3)
project(mara)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")

set(HEADER_FILES
        include/mara.h
        include/Page.h
        include/predefined.h
        include/Statistic.h
        )

set(SOURCE_FILES
        ${HEADER_FILES}
        src/mara.c
        src/Statistic.c
        src/Page.c)

set(CMAKE_INCLUDE_PATH
        ${CMAKE_INCLUDE_PATH}
        library/include
        )

add_library(mara ${SOURCE_FILES})
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/..)