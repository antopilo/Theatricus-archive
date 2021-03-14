#include "Renderer.h"
#include "Shader.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
glm::vec3 Renderer::ClearColor;

Shader* Renderer::QuadShader;
Shader* Renderer::ArtworkShader;

void Renderer::Init()
{
    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW initialization failed!";
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glEnable(GL_DEPTH_TEST);
    
    ClearColor = glm::vec3(0.0f, 0.0f, 0.0f);

    glClearColor(ClearColor.r, ClearColor.g, ClearColor.b, 1.0f);

    QuadShader = new Shader("Res/Shaders/Screen.shader");


}

void Renderer::BeginDraw()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::EndDraw()
{

}

void Renderer::ReloadShader()
{
    QuadShader = new Shader("Res/Shaders/Screen.shader");
}
