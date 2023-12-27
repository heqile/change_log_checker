set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(change_log_checker main.cpp)

target_link_libraries(change_log_checker PRIVATE change_log_checker_lib compiler_flags)