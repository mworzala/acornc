cmake_minimum_required(VERSION 3.22)
project(acorn C)

option(test "Build all tests." OFF)


set(CMAKE_C_STANDARD 11)

#export LDFLAGS="-L/opt/homebrew/opt/llvm@12/lib"
#export CPPFLAGS="-I/opt/homebrew/opt/llvm@12/include"
link_directories(/opt/homebrew/opt/llvm@12/lib)

# Library
file(GLOB_RECURSE SOURCES src/*.c)
file(GLOB_RECURSE HEADERS include/*.h)
add_library(${PROJECT_NAME}_lib STATIC ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME}_lib PUBLIC include)
target_include_directories(${PROJECT_NAME}_lib PUBLIC /opt/homebrew/opt/llvm@12/include)
target_link_libraries(${PROJECT_NAME}_lib PRIVATE LLVM)

# Executable
file(GLOB_RECURSE EXE_SOURCES bin/*.c)
file(GLOB_RECURSE EXE_HEADERS bin/*.h)
add_executable(${PROJECT_NAME} ${EXE_SOURCES} ${EXE_HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC bin)
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_lib)

if (test)
add_subdirectory(test)
endif()

#if (fuzz)
add_subdirectory(fuzzing)
#endif()

## todo apparently this is a cleaner way to add llvm
#cmake_minimum_required(VERSION 3.12)
#
## Find CMake file for Clang
#find_package(Clang REQUIRED)
#
## Add path to LLVM modules
#set(CMAKE_MODULE_PATH
#  ${CMAKE_MODULE_PATH}
#  "${LLVM_CMAKE_DIR}"
#  )
#
## import LLVM CMake functions
#include(AddLLVM)
#
#include_directories(${LLVM_INCLUDE_DIRS})
#include_directories(${CLANG_INCLUDE_DIRS})
#
#add_definitions(${LLVM_DEFINITIONS})
#add_definitions(${CLANG_DEFINITIONS})
#
#add_llvm_executable(myTool main.cpp)
#set_property(TARGET myTool PROPERTY CXX_STANDARD 11)
#target_link_libraries(myTool PRIVATE clangTooling)


## Testing
#if (test)
#    if (APPLE)
#        add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
#        add_definitions(-D__GLIBCXX__)
#    endif (APPLE)
#
#    # Fetch GoogleTest
#    include(FetchContent)
#    FetchContent_Declare(
#            googletest
#            URL https://github.com/google/googletest/archive/5126f7166109666a9c0534021fb1a3038659494c.zip
#    )
#    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
#    FetchContent_MakeAvailable(googletest)
#
#    enable_testing()
#
#    file(GLOB_RECURSE TEST_SOURCES test/**.cc)
#    add_executable(${PROJECT_NAME}_test ${TEST_SOURCES} ${SOURCES})
#    target_link_libraries(${PROJECT_NAME}_test gtest gtest_main)
#    target_include_directories(${PROJECT_NAME}_test PUBLIC include)
##    target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME}_lib)
#
#
#
#    gtest_discover_tests(${PROJECT_NAME}_test)
#
#endif ()



