cmake_minimum_required(VERSION 3.3)
project(mara_test)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")

add_subdirectory(library)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")

set(CMAKE_INCLUDE_PATH
        ${CMAKE_INCLUDE_PATH}
        library/include
        )

add_executable(mara_test testExecutable/main.c)
add_dependencies(mara_test mara)
TARGET_LINK_LIBRARIES(mara_test mara)