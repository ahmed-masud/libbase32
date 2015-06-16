cmake_minimum_required(VERSION 3.2)
project(libbase32)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    LICENSE
    README.md)

add_executable(libbase32 ${SOURCE_FILES} base32.h base32.c)