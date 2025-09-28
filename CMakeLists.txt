cmake_minimum_required(VERSION 3.28)
project(ComputingSystems)

set(CMAKE_CXX_STANDARD 20)

add_executable(ComputingSystems main.cpp
        labs/lab2.cpp
        labs/lab2.h
        labs/lab1.cpp
        labs/lab1.h
        provider.h)
