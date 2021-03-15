#include "Game.h"
#include "Rendering/Renderer.h"
#include "Rendering/Vertex.h"

#include "Artwork/Camera.h"

#include "Core/AnimationPlayer.h"



unsigned int VAO = 0;
unsigned int VBO = 0;
Vertex2d vertices[6]
{
    Vertex2d { {1.0f, 1.0f},   {1.0f, 1.0f} },
    Vertex2d { {1.0f, -1.0f},  {1.0f, -1.0f} },
    Vertex2d { {-1.0f, 1.0f},  {-1.0f, 1.0f} },
    Vertex2d { {1.0f, -1.0f},  {1.0f, -1.0f} },
    Vertex2d { {-1.0f, -1.0f}, {-1.0f, -1.0f} },
    Vertex2d { {-1.0f, 1.0f},  {1.0f, 1.0f} }
};



Game::Game()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2d), (void*)0);
    glEnableVertexAttribArray(0);

    // UV
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2d), (void*)(sizeof(GL_FLOAT) * 2));
    glEnableVertexAttribArray(1);


    m_GameFrameBuffer = new FrameBuffer(1280, 720, true);

    //
    Texture* renderTexture = new Texture({ 1280, 720 }, GL_RGBA, GL_TEXTURE_2D);
    m_GameFrameBuffer->Bind();
    //
    m_GameFrameBuffer->AttachTexture(renderTexture, GL_COLOR_ATTACHMENT0);
    m_GameFrameBuffer->Unbind();

    // Init animation stuff.

    auto animPlayer = AnimationPlayer::Get();

    auto anim = animPlayer->GetAnimation();

    anim->AddKeyframe("test", 0.0f, 1.0f);
    anim->AddKeyframe("test", 0.6f, 3.0f);
    anim->AddKeyframe("hello", 0.5f, 2.0f);

    anim->AddKeyframe("u_testing", 0.0f, 1.0f);
    //anim->RemoveKeyframe("test", 1);
    //anim->RemoveKeyframe("test", 0);
    //anim->RemoveKeyframe("test", 0);
    
}

FrameBuffer* Game::GetFrameBuffer() { return m_GameFrameBuffer;  }

void Game::Draw(Camera* camera)
{
    // Scene
    m_GameFrameBuffer->Bind();
    Renderer::BeginDraw();
    

    // Post-process

    // UI
    Renderer::QuadShader->Bind();

    glm::vec3 pos = camera->GetTranslation();
    glm::vec3 lookAt = camera->GetLookAt(); 
    Renderer::QuadShader->SetUniform3f("camPosition", pos.x, pos.y, pos.z);
    Renderer::QuadShader->SetUniform3f("camLookAt", lookAt.x, lookAt.y, lookAt.z);

    std::vector<std::string> uniforms = AnimationPlayer::Get()->GetAnimation()->GetUniforms();
    for (int i = 0; i < uniforms.size(); i++)
    {
        float value = AnimationPlayer::Get()->GetUniformvalue(uniforms[i], AnimationPlayer::Get()->GetCurrentTime());
        Renderer::QuadShader->SetUniform1f(uniforms[i], value);
    }

    //Renderer::QuadShader->SetUniform3f("camUp", camera->cameraUp.x, camera->cameraUp.y, camera->cameraUp.z);
    //Renderer::QuadShader->SetUniform3f("camRight", camera->cameraRight.x, camera->cameraRight.y, camera->cameraRight.z);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    m_GameFrameBuffer->Unbind();
}