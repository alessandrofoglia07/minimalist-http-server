cmake_minimum_required(VERSION 3.22.1)
project(http_server C)

set(CMAKE_C_STANDARD 11)

include_directories("${PROJECT_SOURCE_DIR}/include")

file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.c")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

add_executable(http_server ${SOURCES})
