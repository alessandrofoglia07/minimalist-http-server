cmake_minimum_required(VERSION 3.28)
project(http_server C)

set(CMAKE_C_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.c")

add_executable(http_server ${SOURCES})
