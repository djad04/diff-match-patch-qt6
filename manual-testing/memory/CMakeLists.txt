cmake_minimum_required(VERSION 3.16)
project(diff-match-patch-memory-test)

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

# Memory test executable
add_executable(memory_test
    memory_test.cpp
    ../../diff_match_patch.cpp
)

target_link_libraries(memory_test Qt6::Core)

# Custom target for easy running
add_custom_target(run
    COMMAND memory_test
    DEPENDS memory_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running memory analysis test"
)
