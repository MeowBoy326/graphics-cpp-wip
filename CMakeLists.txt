cmake_minimum_required(VERSION 3.20)
project(graphics-demos VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(external/glad)
add_subdirectory(external/glfw)

add_executable(viewer src/viewer/main.cpp)
target_link_libraries(viewer glad glfw)