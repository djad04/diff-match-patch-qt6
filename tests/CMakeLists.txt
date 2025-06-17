cmake_minimum_required(VERSION 3.16)
project(diff-match-patch-qt6-tests)

# Find Qt6 with required modules
find_package(Qt6 REQUIRED COMPONENTS Core Network)
find_package(Qt6 COMPONENTS Test QUIET)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt MOC processing
set(CMAKE_AUTOMOC ON)

# Add MSVC-specific flags for Qt6 compatibility
if(MSVC)
    add_compile_options(/Zc:__cplusplus /permissive-)
    add_definitions(-DNOMINMAX)
endif()

# Include parent directory for headers
include_directories(..)

# Enable testing
enable_testing()

# Add subdirectories
add_subdirectory(unit)
add_subdirectory(benchmark)
add_subdirectory(memory)

# Global test target
add_custom_target(run_all_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    COMMENT "Running all tests with CTest"
)
