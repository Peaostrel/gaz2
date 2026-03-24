cmake_minimum_required(VERSION 3.10)
project(gaz2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")

add_executable(gaz2 ${SOURCES})