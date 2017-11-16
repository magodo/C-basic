cmake_minimum_required (VERSION 2.8.5)
project (socket_client)

option (EXPORT_COMPILE_DB
    "Export json compilation database" ON)

if (EXPORT_COMPILE_DB)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif (EXPORT_COMPILE_DB)

add_executable(client client.c)
