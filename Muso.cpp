#include <iostream>
#include "Window.h"
#include "Core/Timestep.h"
float m_LastFrameTime = 0.0f;
int main()
{
    Window window(800, 800, "Hello world");
    
    while (!glfwWindowShouldClose(window.Get()))
    {
        float time = (float)glfwGetTime();
        Timestep timestep = time - m_LastFrameTime;
        m_LastFrameTime = time;
        window.Draw(timestep);
    }

    glfwTerminate();
}