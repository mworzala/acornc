cmake_minimum_required(VERSION 3.22)
project(acornc C)

set(CMAKE_C_STANDARD 99)

file(GLOB_RECURSE SOURCES src/*.c)
file(GLOB_RECURSE HEADERS include/*.h)
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC include)
