add_executable(app main.cpp)

target_link_libraries(app PRIVATE change_log_checker_lib compiler_flags)