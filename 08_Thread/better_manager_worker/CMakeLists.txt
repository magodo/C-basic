cmake_minimum_required (VERSION 2.8.5)
project (test)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(test_interactive ThreadManager.cpp ThreadWorker.cpp test_interactive.cpp)
add_executable(test_inits ThreadManager.cpp ThreadWorker.cpp test_inits.cpp)

target_link_libraries(test_interactive pthread)
target_link_libraries(test_inits pthread)

#foreach (i ${CMAKE_CXX_COMPILE_FEATURES})
#    message("${i}")
#endforeach()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#target_compile_features(test_interactive PRIVATE cxx_std_11)
#target_compile_features(test_inits PRIVATE cxx_std_11)
