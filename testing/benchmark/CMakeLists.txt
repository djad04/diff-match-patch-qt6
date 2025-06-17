cmake_minimum_required(VERSION 3.16)
project(diff-match-patch-benchmark)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add MSVC-specific flags for Qt6 compatibility
if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

# Include parent directories for headers
include_directories(../..)

# Benchmark executable
add_executable(benchmark
    benchmark.cpp
    ../../diff_match_patch.cpp
)

target_link_libraries(benchmark Qt6::Core)

# Set optimization flags for accurate benchmarking
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        target_compile_options(benchmark PRIVATE /O2)
    else()
        target_compile_options(benchmark PRIVATE -O3)
    endif()
endif()

# Custom target for easy running
add_custom_target(run
    COMMAND benchmark
    DEPENDS benchmark
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running performance benchmark"
)
