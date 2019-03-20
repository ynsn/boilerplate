cmake_minimum_required(VERSION 3.13)
project(GLAD)

set(CMAKE_CXX_STANDARD 14)

set(GLAD_HEADERS include/glad/glad.h include/KHR/khrplatform.h)
set(GLAD_SOURCES src/glad.c)

add_library(glad ${GLAD_HEADERS} ${GLAD_SOURCES})
target_include_directories(glad PUBLIC include)