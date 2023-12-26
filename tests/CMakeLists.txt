add_subdirectory("${PROJECT_SOURCE_DIR}/external/googletest" "external/googletest")

add_executable(run_test_change_log_checker test_change_log_checker.cpp)

target_link_libraries(run_test_change_log_checker PRIVATE GTest::gtest_main)

gtest_discover_tests(
    run_test_change_log_checker 
)

