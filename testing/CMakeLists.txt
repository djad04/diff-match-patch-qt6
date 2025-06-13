cmake_minimum_required(VERSION 3.16)
project(diff-match-patch-qt6-tests)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add MSVC-specific flags for Qt6 compatibility
if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

# Include parent directory for headers
include_directories(..)

# Benchmark executable
add_executable(benchmark
    benchmark.cpp
    ../diff_match_patch.cpp
)

target_link_libraries(benchmark Qt6::Core)

# Test suite executable
add_executable(test_suite
    test_suite.cpp
    ../diff_match_patch.cpp
)

target_link_libraries(test_suite Qt6::Core)

# Patch fix test executable
add_executable(patch_fix_test
    patch_fix_test.cpp
    ../diff_match_patch.cpp
)

target_link_libraries(patch_fix_test Qt6::Core)

# Memory test executable (if available)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_executable(memory_test
        memory_test.cpp
        ../diff_match_patch.cpp
    )
    target_link_libraries(memory_test Qt6::Core)
endif()

# Custom targets for easy running
add_custom_target(run_benchmark
    COMMAND benchmark
    DEPENDS benchmark
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running performance benchmark"
)

add_custom_target(run_tests
    COMMAND test_suite
    DEPENDS test_suite
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running test suite"
)

add_custom_target(run_all
    COMMAND test_suite && benchmark
    DEPENDS test_suite benchmark
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running all tests and benchmarks"
)
