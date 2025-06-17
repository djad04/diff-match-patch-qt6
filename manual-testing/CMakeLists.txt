cmake_minimum_required(VERSION 3.16)
project(diff-match-patch-qt6-tests)

# Option to choose which test to build
option(BUILD_BENCHMARK "Build performance benchmark" OFF)
option(BUILD_CORRECTNESS "Build correctness test suite" OFF)
option(BUILD_MEMORY "Build memory analysis test" OFF)
option(BUILD_ALL "Build all tests" ON)

# If no specific test is chosen, build all
if(NOT BUILD_BENCHMARK AND NOT BUILD_CORRECTNESS AND NOT BUILD_MEMORY)
    set(BUILD_ALL ON)
endif()

# Add subdirectories based on options
if(BUILD_ALL OR BUILD_BENCHMARK)
    message(STATUS "Adding benchmark test")
    add_subdirectory(benchmark)
endif()

if(BUILD_ALL OR BUILD_CORRECTNESS)
    message(STATUS "Adding correctness test")
    add_subdirectory(correctness)
endif()

if(BUILD_ALL OR BUILD_MEMORY)
    message(STATUS "Adding memory test")
    add_subdirectory(memory)
endif()

# Global targets that work across subdirectories
if(BUILD_ALL)
    add_custom_target(run_all_tests
        COMMENT "Running all tests and benchmarks"
    )

    # Add dependencies if targets exist
    if(TARGET benchmark)
        add_dependencies(run_all_tests benchmark)
    endif()
    if(TARGET test_suite)
        add_dependencies(run_all_tests test_suite)
    endif()
    if(TARGET memory_test)
        add_dependencies(run_all_tests memory_test)
    endif()
endif()
