#include <iostream>
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;

void handle_glfw_error(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

GLFWwindow* initialize_glfw() {
 if (!glfwInit()) {
        std::cerr << "GLFW initialization failed!" << std::endl;
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwSetErrorCallback(handle_glfw_error);

    auto window = glfwCreateWindow(1280, 720, "Viewer Demo", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window!" << std::endl;
    }
    glfwMakeContextCurrent(window);

    return window;
}

void terminate_glfw(GLFWwindow* window) {
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }

    glfwTerminate();
}

bool initialize_gl() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD initialization failed!" << std::endl;
        return false;
    }

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glClearColor(0, 0, 0, 0);


    return true;
}

int main(void) {

    // Initialization
    std::cout << "Hello viewer!" << std::endl;

    auto window = initialize_glfw();
    if (!window) {
        return -1;
    }

    if (!initialize_gl()) {
        return -1;
    }

    bool running = true;
    while (running) {

        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            running = false;
        }

    }




    // Cleanup
    terminate_glfw(window);


    return 0;
}