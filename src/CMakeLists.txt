# add_executable(app main.cpp)

# # target_compile_features(app PRIVATE cxx_std_20)
# target_link_libraries(app compiler_flags)

add_library(change_log_checker_lib STATIC change_log_checker_lib.cpp)

target_include_directories(change_log_checker_lib PUBLIC ../include)
target_link_libraries(change_log_checker_lib PRIVATE compiler_flags)
