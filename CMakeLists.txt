cmake_minimum_required(VERSION 2.8.12)
project(Simple_Shell C)

set(CMAKE_C_STANDARD 11)
include_directories(/usr/include/readline)
add_executable(Simple_Shell
        exec.c
        exec.h
        main.c)
target_link_libraries(Simple_Shell readline)
