// Should only be accessed from drawing devices

#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class OpenGLDevice{
public:
    OpenGLDevice();
    ~OpenGLDevice();
    bool shouldTerminate = false;

    
private:
    GLFWwindow* window;
    unsigned int shaderProgram;
    unsigned int VBO, VAO;
    
    float vertices[8] = {
        -0.5f, -0.5f,
        -0.5f,  0.5f,
         0.5f,  0.5f,
         0.5f, -0.5f
    };
    
    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 600;
    const char* TITLE = "emu6502";
    
    // Shaders
    const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
    "}\0";
    const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

    void processInput();
    
    public:
    void render();
    void update(float newVert[8]);
};