cmake_minimum_required(VERSION 3.6)
project(all)

set(CMAKE_CXX_STANDARD 17)

unset(CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES)
include_directories("util")
include_directories("assembler/src")
include_directories("processor/src")

add_subdirectory("assembler" "assembler/out")
add_subdirectory("processor" "processor/out")
