cmake_minimum_required(VERSION 3.20)
project(Snake C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(Snake
        game.h
        main.c
        structure.h)
