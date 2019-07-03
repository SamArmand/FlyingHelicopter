cmake_minimum_required(VERSION 3.6)
project(FlyingHelicopter)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        FlyingHelicopter/main.cpp)

add_executable(FlyingHelicopter ${SOURCE_FILES})