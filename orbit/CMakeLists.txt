cmake_minimum_required(VERSION 3.17)

project(orbit)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

include_directories("include" "include/glm" "include/imgui")

FILE(GLOB IMGUISRC "include/imgui/*.cpp" "include/imgui/misc/cpp/*.cpp" "include/imgui/backends/imgui_impl_glfw.cpp" "include/imgui/backends/imgui_impl_opengl3.cpp")

add_executable(${PROJECT_NAME} ${IMGUISRC} "src/glad.c" "src/main.cpp" "src/graphics/shader.cpp" "src/graphics/texture.cpp" "src/graphics/index_buffer.cpp" "src/graphics/vertex_buffer.cpp" "src/graphics/vertex_array.cpp")

target_link_libraries(${PROJECT_NAME} GL dl glfw)
