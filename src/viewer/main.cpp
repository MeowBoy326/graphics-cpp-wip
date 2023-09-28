#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

#include <glm/common.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
    glDepthFunc(GL_LEQUAL);

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




class Buffer {

    private:
    GLuint id;

    public:

    Buffer(const std::vector<unsigned char> & data) {
        glCreateBuffers(1, &id);
        glBindBuffer(GL_ARRAY_BUFFER, id);
        glBufferData(GL_ARRAY_BUFFER, data.size(), data.data(), GL_STATIC_DRAW);
    }

    uint32_t get_id() const {
        return id;
    }



};

class Mesh {

    private:
    GLuint vao;
    size_t vertex_count;
    size_t indices_offset;

    // void set_attribute(uint32_t id, uint32_t num_components, uint32_t stride, uint32_t byte_offset) {
    //     glBindVertexArray(vao);
    //     glVertexAttribPointer(id, num_components, )
    //     glEnableVertexArrayAttrib(vao, id);

    // }

    public:

    Mesh() {
        glCreateVertexArrays(1, &vao);
        vertex_count = 0;
        indices_offset = 0;
    }

    // void set_vertex_count(size_t count) {
    //     vertex_count = count;
    // }

    void set_position_attribute(Buffer* buffer, uint32_t stride, uint32_t offset) {
        uint32_t id = 0;
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, buffer->get_id());
        glVertexAttribPointer(id, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
        glEnableVertexArrayAttrib(vao, id);
    }

    void set_normal_attribute(Buffer* buffer, uint32_t stride, uint32_t offset) {
        uint32_t id = 1;
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, buffer->get_id());
        glVertexAttribPointer(id, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
        glEnableVertexArrayAttrib(vao, id);
    }

    void set_indices(Buffer* buffer, uint32_t count, uint32_t offset) {
        glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->get_id());
        vertex_count = count;
        indices_offset = offset;
    }

    // TODO: Better abstractions
    void draw() {
        glDrawElements(GL_TRIANGLES, vertex_count, GL_UNSIGNED_SHORT, (void*)indices_offset);
    }

};

class Scene {
    
    public:

    std::vector<std::unique_ptr<Mesh>> meshes;
    std::vector<std::unique_ptr<Buffer>> buffers;
};

std::unique_ptr<Scene> load_scene(const char* path) {    
    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string err, warn;
    
    auto success = loader.LoadASCIIFromFile(&model, &err, &warn, path);
    
    if (!warn.empty()) {
        std::cerr << "TinyGLTF Warning: " << warn << std::endl;
    }
    
    if (!err.empty()) {
        std::cerr << "TinyGLTF Error: " << err << std::endl;
    }

    if (success) {

        // handle the default scene only
        const auto & s = model.scenes.at(model.defaultScene);
        auto scene = std::make_unique<Scene>();

        // create the buffers
        for (const auto & b: model.buffers) {
            auto buffer = std::make_unique<Buffer>(b.data);
            scene->buffers.push_back(std::move(buffer));
        }
        // model.buffers


        // prepare data for all meshes
        for (const auto & m: model.meshes) {
            auto mesh = std::make_unique<Mesh>();

            // just assume triangles rendering mode and only one primitive
            // create all attributes
            // just deal with position and normals for now

            for (const auto & [attribute, accessor_idx]: m.primitives.at(0).attributes) {
                // for each attribute, we need offset, type, normalized, and stride
                const auto & accessor = model.accessors.at(accessor_idx);
                const auto offset = accessor.byteOffset + model.bufferViews.at(accessor.bufferView).byteOffset;
                const auto stride = model.bufferViews.at(accessor.bufferView).byteStride;
                auto buffer = scene->buffers[model.bufferViews[accessor.bufferView].buffer].get();
                // assume normalized is false
                if (attribute == "POSITION") {
                    // assume vec3
                    mesh->set_position_attribute(buffer, stride, offset);
                    // mesh->set_vertex_count(accessor.count);

                }
                else if (attribute == "NORMAL") {
                    // assume vec3
                    mesh->set_normal_attribute(buffer, stride, offset);
                }
            }

            // deal with indices

            {
                const auto & accessor = model.accessors.at(m.primitives.at(0).indices);
                const auto offset = accessor.byteOffset + model.bufferViews.at(accessor.bufferView).byteOffset;
                // const auto stride = model.bufferViews.at(accessor.bufferView).byteStride;
                auto buffer = scene->buffers[model.bufferViews[accessor.bufferView].buffer].get();
                const auto count = accessor.count;

                mesh->set_indices(buffer, count, offset);

            }

            scene->meshes.push_back(std::move(mesh));
        
        }


        return scene;

    }

    std::cerr << "Failed to load GLTF model!" << std::endl;

    return nullptr;
}



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

    auto scene = load_scene("../../models/Cube/Cube.gltf");

    bool running = true;
    while (running)
    {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // set uniforms
        glm::mat4 model(1.0);
        glm::mat4 view = glm::lookAt(glm::vec3(5.0, 5.0, 5.0), glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 projection = glm::perspective(45.0, 16.0/9.0, 0.1, 1000.0);

        glUniformMatrix4fv(0, 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(1, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(2, 1, GL_FALSE, &projection[0][0]);


        scene->meshes.at(0)->draw();


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