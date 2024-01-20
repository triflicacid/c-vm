cmake_minimum_required(VERSION 3.6)
project(processor LANGUAGES C)
set(CMAKE_VERBOSE_MAKEFILE ON)

file(GLOB src "src/*.c" "../util/*.c")

include_directories("src")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
add_executable(processor "main.c" ${src})
