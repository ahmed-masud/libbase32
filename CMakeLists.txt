cmake_minimum_required(VERSION 2.8)
project(libbase32)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    base32.h
    base32.c
    LICENSE
    README.md)

add_library(base32 STATIC ${SOURCE_FILES})
add_executable(base32enc main.c)


add_dependencies(base32enc base32)
target_link_libraries(base32enc base32)
