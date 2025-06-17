# Performance benchmark tests

add_executable(test_performance
    test_performance.cpp
    ../../diff_match_patch.cpp
)
target_link_libraries(test_performance Qt6::Core Qt6::Network)
if(TARGET Qt6::Test)
    target_link_libraries(test_performance Qt6::Test)
endif()

# Set optimization flags for accurate benchmarking
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        target_compile_options(test_performance PRIVATE /O2)
    else()
        target_compile_options(test_performance PRIVATE -O3)
    endif()
endif()

# Add as test but with longer timeout for performance tests
add_test(NAME PerformanceBenchmark COMMAND test_performance)
set_tests_properties(PerformanceBenchmark PROPERTIES TIMEOUT 300)
