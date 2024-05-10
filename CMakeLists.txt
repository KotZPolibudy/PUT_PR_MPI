cmake_minimum_required(VERSION 3.24)
project(PUT_PR_MPI)

set(CMAKE_CXX_STANDARD 14)

include_directories(projekt)

add_executable(PUT_PR_MPI
        projekt/communication.cpp
        projekt/communication.h
        projekt/main.cpp
        projekt/tags.h
        projekt/utils.cpp
        projekt/utils.h)
