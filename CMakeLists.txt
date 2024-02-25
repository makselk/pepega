cmake_minimum_required(VERSION 3.16)
project(ppega)

set(CMAKE_CXX_STANDARD 17)

add_executable(ppega ipega.cpp main.cpp)

target_link_libraries(ppega bluetooth)