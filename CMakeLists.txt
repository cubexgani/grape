cmake_minimum_required(VERSION 3.14)
project(grape VERSION 1.1 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB SOURCES CONFIGURE_DEPENDS "src/*.c")
add_executable(grape ${SOURCES})

target_include_directories(grape PRIVATE include)
target_compile_options(grape PRIVATE -Wall -Wextra -Wpedantic -Werror)