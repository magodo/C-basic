cmake_minimum_required (VERSION 2.8.5)
project (test)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(getnameinfo ./a.cpp)
