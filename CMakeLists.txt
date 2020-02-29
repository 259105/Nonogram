cmake_minimum_required(VERSION 3.15)
project(Nonogram C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W -Wall -Werror -Wno-error=comment -Wextra -pedantic")

add_executable(Nonogram main.c Nonogram.c)