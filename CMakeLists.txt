cmake_minimum_required(VERSION 3.10.3)
project(Rendor)

# Build GLFW as a static library
set(BUILD_SHARED_LIBS OFF CACHE "Build shared libraries" INTERNAL FORCE)

add_subdirectory(deps/assimp)
add_subdirectory(deps/glad)
add_subdirectory(deps/glfw)
add_subdirectory(deps/glm)
add_subdirectory(deps/meshoptimizer)

set(IMGUI_HEADERS deps/imgui/imgui.h deps/imgui/examples/imgui_impl_opengl3.h deps/imgui/examples/imgui_impl_glfw.h deps/imgui/misc/cpp/imgui_stdlib.h include/cg/GUIComponent.h include/cg/common/VertexArray.h include/cg/common/Bindable.h include/cg/common/Handlable.h include/cg/Mesh.h include/cg/InfoImporter.h)
set(IMGUI_SOURCES deps/imgui/imgui.cpp deps/imgui/imgui_demo.cpp deps/imgui/imgui_draw.cpp deps/imgui/imgui_widgets.cpp deps/imgui/examples/imgui_impl_opengl3.cpp deps/imgui/examples/imgui_impl_glfw.cpp deps/imgui/misc/cpp/imgui_stdlib.cpp)

add_library(imgui STATIC ${IMGUI_HEADERS} ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC deps/imgui deps/glad/include deps/glfw/include)
target_compile_definitions(imgui PUBLIC "IMGUI_IMPL_OPENGL_LOADER_GLAD")

set(RENDOR_HEADERS include/cg/Application.h include/cg/common/Shader.h include/cg/common/Program.h include/cg/Vertex.h)
set(RENDOR_SOURCES lib/Application.cpp lib/common/Shader.cpp lib/common/Program.cpp lib/InfoImporter.cpp)

set(CMAKE_CXX_STANDARD 14)

add_library(rendor STATIC ${RENDOR_HEADERS} ${RENDOR_SOURCES})
target_include_directories(rendor PUBLIC include deps/glfw/include deps/glad/include deps/glm deps/assimp/include)
target_link_libraries(rendor PUBLIC opengl32 glfw glad assimp imgui meshoptimizer)

add_subdirectory(tests)