cmake_minimum_required(VERSION 3.22)
project(acornc C)

option(test "Build all tests." OFF)


set(CMAKE_C_STANDARD 11)

# Library
file(GLOB_RECURSE SOURCES src/*.c)
file(GLOB_RECURSE HEADERS include/*.h)
add_library(${PROJECT_NAME}_lib ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME}_lib PUBLIC include)

# Executable
add_executable(${PROJECT_NAME} bin/main.c)
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_lib)

add_subdirectory(test)

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
#    include(GoogleTest)
#    gtest_discover_tests(${PROJECT_NAME}_test)
#
#endif ()



