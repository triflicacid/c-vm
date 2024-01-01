cmake_minimum_required(VERSION 3.6)
project(assembler)

file(GLOB src "src/*.c" "../util/*.c")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
add_executable(assembler "main.c" ${src} "../processor/src/registers.c")