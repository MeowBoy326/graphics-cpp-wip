cmake_minimum_required(VERSION 3.20)
project(graphics-demos VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(external/glad)
add_subdirectory(external/glfw)
add_subdirectory(external/glm)

add_executable(viewer src/viewer/main.cpp)
target_link_libraries(viewer glad glfw glm)
target_include_directories(viewer PRIVATE include)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders/
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/shaders/)