project(acorn_test CXX)

set(CMAKE_CXX_STANDARD 11)

# Fetch GoogleTest
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/5126f7166109666a9c0534021fb1a3038659494c.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Create test executable
file(GLOB_RECURSE TEST_SOURCES src/**.cc)
add_executable(acorn_test_run ${TEST_SOURCES})
target_link_libraries(acorn_test_run gtest gtest_main)
target_link_libraries(acorn_test_run acorn_lib)

include(GoogleTest)
gtest_discover_tests(acorn_test_run)