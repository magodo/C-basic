cmake_minimum_required (VERSION 2.8.5)
project (foobar)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(foobar ThreadManager.cpp ThreadWorker.cpp main.cpp)
target_link_libraries(foobar pthread)
target_compile_features(foobar PRIVATE cxx_std_11)
