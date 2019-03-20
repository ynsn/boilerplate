cmake_minimum_required(VERSION 3.13)
project(Rendor)

link_libraries(rendor)
include_directories(../include)

add_executable(triangle triangle.cpp)