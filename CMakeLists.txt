project(graphics-demos)
cmake_minimum_required(VERSION 3.20)

add_subdirectory(external/glad)

add_executable(viewer src/viewer/main.cpp)
target_link_libraries(viewer glad)