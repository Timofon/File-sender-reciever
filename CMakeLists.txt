cmake_minimum_required(VERSION 3.23)
project(caos_project C)

set(CMAKE_C_STANDARD 99)

find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})

add_executable(caos_project main.c RLE.h zlib_compress.h)
target_link_libraries(caos_project ${ZLIB_LIBRARIES})