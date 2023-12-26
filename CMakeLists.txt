cmake_minimum_required(VERSION 3.16)

project(changeLogChecker 
    VERSION 0.1 
    LANGUAGES CXX
)

add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_20)

add_subdirectory(src)
add_subdirectory(app)
