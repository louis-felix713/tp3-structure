cmake_minimum_required(VERSION 3.5)
project(TP3)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    Dictionnaire.cpp
    Dictionnaire.h
    Principal.cpp)

add_executable(TP3 ${SOURCE_FILES})
