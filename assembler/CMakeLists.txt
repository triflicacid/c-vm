cmake_minimum_required(VERSION 3.6)
project(assembler LANGUAGES CXX)

include_directories("src")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
add_executable(assembler "../util/util.cpp" "src/chunk.cpp" "src/assembler_data.cpp" "src/parser.cpp"
        "src/instructions/argument.cpp" "src/instructions/instruction.cpp" "src/instructions/signature.cpp"
        "src/instructions/signatures.cpp" "src/messages/message.cpp" "src/messages/error.cpp" "src/messages/list.cpp"
        "src/pre-process/data.cpp" "src/pre-process/line.cpp" "src/pre-process/pre-processor.cpp" "assembler.cpp")


project(disassembler LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
add_executable(disassembler "../util/util.cpp" "src/disassembler_data.cpp" "disassembler.cpp")
