cmake_minimum_required(VERSION 3.13)
project(Rendor)

add_subdirectory(deps/glad)
add_subdirectory(deps/glfw)

set(RENDOR_HEADERS include/rendor/Application.h)
set(RENDOR_SOURCES lib/Application.cpp)

set(CMAKE_CXX_STANDARD 14)

add_library(rendor ${RENDOR_HEADERS} ${RENDOR_SOURCES})
target_include_directories(rendor PUBLIC include deps/glfw/include deps/glad/include)
target_link_libraries(rendor PUBLIC glfw glad)

add_subdirectory(tests)