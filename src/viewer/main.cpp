#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;

void handle_glfw_error(int error, const char *description)
{
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

// See https://www.khronos.org/opengl/wiki/OpenGL_Error
// TODO: Rewrite
void GLAPIENTRY
MessageCallback(GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar *message,
                const void *userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
}

GLFWwindow *initialize_glfw()
{
    if (!glfwInit())
    {
        std::cerr << "GLFW initialization failed!" << std::endl;
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, false);

    glfwSetErrorCallback(handle_glfw_error);

    auto window = glfwCreateWindow(1280, 720, "Viewer Demo", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create window!" << std::endl;
    }

    glfwMakeContextCurrent(window);

    return window;
}

void terminate_glfw(GLFWwindow *window)
{
    if (window)
    {
        glfwDestroyWindow(window);
        window = nullptr;
    }

    glfwTerminate();
}

bool initialize_gl()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "GLAD initialization failed!" << std::endl;
        return false;
    }

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glClearColor(0, 0, 0, 0);

    return true;
}

class ShaderProgram
{

private:
    bool valid;

    std::string read_file(const char *path)
    {
        std::ostringstream ss;
        std::ifstream file(path);
        // std::cout << file.is_open() << std::endl;
        ss << file.rdbuf();
        return ss.str();
    }

public:
    // TODO: Refactor and add error checking
    ShaderProgram(const char *vert, const char *frag) : valid(false)
    {

        // read shader source code into strings
        const std::string vs_source = read_file(vert);
        const std::string fs_source = read_file(frag);
        // std::cout << vs_source << std::endl;

        const char *vs_ptr = vs_source.c_str();
        const char *fs_ptr = fs_source.c_str();

        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(vertex_shader, 1, &vs_ptr, nullptr);
        glShaderSource(fragment_shader, 1, &fs_ptr, nullptr);

        glCompileShader(vertex_shader);
        glCompileShader(fragment_shader);

        program = glCreateProgram();
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);

        glLinkProgram(program);

        glValidateProgram(program);

        GLint status;
        glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
        valid = status == GL_TRUE;
    }
    const bool is_valid() const { return valid; }

    GLuint program;
    GLuint vertex_shader;
    GLuint fragment_shader;
};

int main(void)
{

    // Initialization
    std::cout << "Hello viewer!" << std::endl;

    auto window = initialize_glfw();
    if (!window)
    {
        return -1;
    }

    if (!initialize_gl())
    {
        return -1;
    }

    auto prog = ShaderProgram("../shaders/shader.vert.glsl", "../shaders/shader.frag.glsl");
    if (!prog.is_valid())
    {
        std::cerr << "Shader program creation failed!" << std::endl;
        return -1;
    }

    glUseProgram(prog.program);

    bool running = true;
    while (running)
    {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwWindowShouldClose(window))
        {
            running = false;
        }
    }

    // Cleanup
    terminate_glfw(window);

    return 0;
}