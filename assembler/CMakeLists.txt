cmake_minimum_required(VERSION 3.6)
project(assembler LANGUAGES C CXX)

file(GLOB_RECURSE src "../util/*.c" "src/*.cpp")

include_directories("src")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
add_executable(assembler ${src} "main.cpp")
