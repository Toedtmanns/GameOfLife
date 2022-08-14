cmake_minimum_required(VERSION 3.12)

project(GameOfLife VERSION 1.0.0 LANGUAGES C)

add_compile_definitions(PROG_VERSION=${GameOfLife_VERSION})

if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

add_executable(GameOfLife
    ./source/main.c
    ./source/gol.c
    ./source/commands.c)

target_include_directories(GameOfLife PUBLIC ./include)