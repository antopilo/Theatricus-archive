#pragma once
#include "glm/vec3.hpp"
#include "Shader.h"

class Renderer
{
private:
    static Renderer* m_Instance;
    static glm::vec3 ClearColor;
    
public:
    static Shader* QuadShader;
    static Shader* ArtworkShader;

    static void Init();
    static void BeginDraw();
    static void EndDraw();

    static void ReloadShader();
};