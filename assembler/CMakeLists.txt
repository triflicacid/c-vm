cmake_minimum_required(VERSION 3.6)
project(assembler LANGUAGES C)

file(GLOB src "src/*.c" "../util/*.c")

include_directories("src")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
add_executable(assembler "main.c" ${src})