cmake_minimum_required(VERSION 3.11)
project(raw-pkt-gen)

set(CMAKE_C_FLAGS "-O3 -g -flto -std=c11 -Wall -D_POSIX_C_SOURCE=199309L")
set(CMAKE_CXX_FLAGS "-O3 -g -flto -std=c++11 -Wall -D_POSIX_C_SOURCE=199309L")
set(LD_FLAGS "-flto")

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

add_subdirectory(lib)
add_subdirectory(apps)

