cmake_minimum_required(VERSION 3.6)
project(language LANGUAGES CXX)

include_directories("src")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin)
add_executable(language ../util/messages/message.cpp ../util/messages/MessageWithSource.cpp ../util/messages/list.cpp
        src/lexer/Lexer.cpp src/lexer/Token.cpp main.cpp)

