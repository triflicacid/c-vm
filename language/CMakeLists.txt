cmake_minimum_required(VERSION 3.6)
project(language LANGUAGES CXX)

include_directories("src")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin)
add_executable(language ../util/messages/message.cpp ../util/messages/MessageWithSource.cpp ../util/messages/list.cpp
        src/Source.cpp src/lexer/Lexer.cpp src/lexer/Token.cpp src/parser/Symbol.cpp src/parser/Scope.cpp src/parser/ScopeManager.cpp
        src/parser/Parser.cpp src/types/UserType.cpp src/types/FunctionType.cpp src/statement/Function.cpp
        src/Program.cpp main.cpp)

