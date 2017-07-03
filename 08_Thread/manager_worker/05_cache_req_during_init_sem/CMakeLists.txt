cmake_minimum_required (VERSION 2.8.5)
project (test)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SEM_ROOT ../../semaphore)

include_directories(pub)
include_directories(src)
include_directories(${SEM_ROOT}/pub)
include_directories(${SEM_ROOT}/src)

add_executable(test_interactive src/ThreadManager.cpp src/ThreadWorker.cpp test/test_interactive.cpp
    ${SEM_ROOT}/src/CvSemaphore.cpp
    ${SEM_ROOT}/src/PosixSemaphore.cpp
    ${SEM_ROOT}/src/IUnNamedSemaphore.cpp)

target_link_libraries(test_interactive pthread)

add_executable(test_automatic src/ThreadManager.cpp src/ThreadWorker.cpp test/test_automatic.cpp
    ${SEM_ROOT}/src/CvSemaphore.cpp
    ${SEM_ROOT}/src/PosixSemaphore.cpp
    ${SEM_ROOT}/src/IUnNamedSemaphore.cpp)
target_link_libraries(test_automatic pthread)

#foreach (i ${CMAKE_CXX_COMPILE_FEATURES})
#    message("${i}")
#endforeach()

