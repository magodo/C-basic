cmake_minimum_required (VERSION 2.8.5)
project (semaphore)

set(Lib Semaphore)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(pub)
include_directories(src)

add_library(${Lib} SHARED src/IUnNamedSemaphore.cpp src/CvSemaphore.cpp src/PosixSemaphore.cpp)

target_link_libraries(${Lib} pthread)

# Tests
add_executable(test_wait test/test_wait.cpp)
add_executable(test_try_wait test/test_trywait.cpp)
target_link_libraries(test_wait ${Lib})
target_link_libraries(test_try_wait ${Lib})
