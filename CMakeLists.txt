cmake_minimum_required(VERSION 3.3)
project(mara)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")

set(HEADER_FILES
    include/interface.h
        include/Page.h
        include/predefined.h
        #include/Statistic.h
        )

set(SOURCE_FILES
        ${HEADER_FILES}
        main.c
        src/interface.c
        #src/Statistic.c
        src/Page.c)

set(CMAKE_INCLUDE_PATH
        ${CMAKE_INCLUDE_PATH}
        include
        )
add_executable(mara ${SOURCE_FILES})
