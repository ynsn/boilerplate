cmake_minimum_required(VERSION 3.10.3)
project(Rendor)

# Build GLFW as a static library
set(BUILD_SHARED_LIBS OFF CACHE "Build shared libraries" INTERNAL FORCE)

add_subdirectory(deps/assimp)
add_subdirectory(deps/glad)
add_subdirectory(deps/glfw)
add_subdirectory(deps/glm)

set(RENDOR_HEADERS include/cg/Application.h include/cg/common/Shader.h include/cg/common/Program.h)
set(RENDOR_SOURCES lib/Application.cpp lib/common/Shader.cpp lib/common/Program.cpp)

set(CMAKE_CXX_STANDARD 14)

add_library(rendor STATIC ${RENDOR_HEADERS} ${RENDOR_SOURCES})
target_include_directories(rendor PUBLIC include deps/glfw/include deps/glad/include deps/glm deps/assimp/include)
target_link_libraries(rendor PUBLIC opengl32 glfw glad)

add_subdirectory(tests)