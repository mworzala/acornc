project(acornc_test CXX)

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_FLAGS "-g -Wall")

add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(acornc_test_run src/hello_test.cc src/lexer/lex_individual_tokens.cc)
target_link_libraries(acornc_test_run gtest gtest_main)
target_link_libraries(acornc_test_run acornc_lib)
