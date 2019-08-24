cmake_minimum_required(VERSION 3.0.0)
project(HighloadServer C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

find_package(Threads)

add_executable(HighloadServer src/main.c src/server.c src/log.c include/server.h include/log.h include/config.h)

target_link_libraries(HighloadServer event)
target_link_libraries(HighloadServer ${CMAKE_THREAD_LIBS_INIT} )
