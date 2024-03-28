cmake_minimum_required(VERSION 3.6)
project(processor LANGUAGES C)

include_directories("src")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin)
add_executable(processor ../util/util.c src/bit-ops.c src/cpu.c main.c)
