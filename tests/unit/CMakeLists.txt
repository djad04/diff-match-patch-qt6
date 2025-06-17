# Unit tests using QTest framework

# Test for diff operations
add_executable(test_diff
    test_diff.cpp
    ../../diff_match_patch.cpp
)
target_link_libraries(test_diff Qt6::Core Qt6::Network)
if(TARGET Qt6::Test)
    target_link_libraries(test_diff Qt6::Test)
endif()
add_test(NAME DiffOperations COMMAND test_diff)

# Test for patch operations  
add_executable(test_patch
    test_patch.cpp
    ../../diff_match_patch.cpp
)
target_link_libraries(test_patch Qt6::Core Qt6::Network)
if(TARGET Qt6::Test)
    target_link_libraries(test_patch Qt6::Test)
endif()
add_test(NAME PatchOperations COMMAND test_patch)

# Test for match operations
add_executable(test_match
    test_match.cpp
    ../../diff_match_patch.cpp
)
target_link_libraries(test_match Qt6::Core Qt6::Network)
if(TARGET Qt6::Test)
    target_link_libraries(test_match Qt6::Test)
endif()
add_test(NAME MatchOperations COMMAND test_match)

# Test for edge cases
add_executable(test_edge_cases
    test_edge_cases.cpp
    ../../diff_match_patch.cpp
)
target_link_libraries(test_edge_cases Qt6::Core Qt6::Network)
if(TARGET Qt6::Test)
    target_link_libraries(test_edge_cases Qt6::Test)
endif()
add_test(NAME EdgeCases COMMAND test_edge_cases)
