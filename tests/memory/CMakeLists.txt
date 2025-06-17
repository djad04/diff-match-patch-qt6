# Memory usage tests

add_executable(test_memory
    test_memory.cpp
    ../../diff_match_patch.cpp
)
target_link_libraries(test_memory Qt6::Core Qt6::Network)
if(TARGET Qt6::Test)
    target_link_libraries(test_memory Qt6::Test)
endif()

# Add as test with longer timeout
add_test(NAME MemoryUsage COMMAND test_memory)
set_tests_properties(MemoryUsage PROPERTIES TIMEOUT 180)
