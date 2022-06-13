#set(CMAKE_C_COMPILER /opt/homebrew/opt/llvm@12/bin/clang)
#set(CMAKE_CXX_COMPILER /opt/homebrew/opt/llvm@12/bin/clang)

project(acorn_fuzzing CXX)

add_executable(acorn_fuzzing fuzz_target.cc)
target_compile_options(acorn_fuzzing PRIVATE -g -O1 -fsanitize=fuzzer)

target_link_libraries(acorn_fuzzing PRIVATE -fsanitize=fuzzer)
