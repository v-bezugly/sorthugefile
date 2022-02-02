cmake_minimum_required(VERSION 3.0)

project(sort_huge VERSION 1.2.3.4 LANGUAGES C CXX)

add_compile_options("-std=c++14" "-D_NDEBUG" "-O3" "-lpthread")

include_directories(".")

add_executable(${PROJECT_NAME} main.cpp sort_huge_file.cpp chunk.cpp)
