cmake_minimum_required(VERSION 3.16)
project(diff-match-patch-tests)

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

# Test suite executable
add_executable(test_suite
    test_suite.cpp
    ../../diff_match_patch.cpp
)

target_link_libraries(test_suite Qt6::Core)

# Custom target for easy running
add_custom_target(run
    COMMAND test_suite
    DEPENDS test_suite
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running correctness test suite"
)
