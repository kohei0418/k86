cmake_minimum_required(VERSION 3.17)
project(k86 C)

set(CMAKE_C_STANDARD 99)

add_executable(k86 main.c instructions.c modrm.c)