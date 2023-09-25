#include <iostream>
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;

void handle_glfw_error(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

int main(void) {

    // Initialization
    std::cout << "Hello viewer!" << std::endl;

    if (!glfwInit()) {
        std::cerr << "GLFW initialization failed!" << std::endl;
        return 1;
    }
    glfwSetErrorCallback(handle_glfw_error);

    auto window = glfwCreateWindow(1280, 720, "Viewer Demo", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window!" << std::endl;
        return 1;
    }




    // Cleanup

    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }

    glfwTerminate();

    return 0;
}