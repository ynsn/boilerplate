cmake_minimum_required(VERSION 3.10.3)
project(Rendor)

add_subdirectory(deps/glad)
add_subdirectory(deps/glfw)
add_subdirectory(deps/glm)

set(RENDOR_HEADERS include/rendor/Application.h include/rendor/common/Shader.h include/rendor/common/Program.h)
set(RENDOR_SOURCES lib/Application.cpp lib/common/Shader.cpp lib/common/Program.cpp)

set(CMAKE_CXX_STANDARD 14)

add_library(rendor ${RENDOR_HEADERS} ${RENDOR_SOURCES})
target_include_directories(rendor PUBLIC include deps/glfw/include deps/glad/include deps/glm)
target_link_libraries(rendor PUBLIC glfw glad)

add_subdirectory(tests)