#include "polos/polos_pch.h"

#include "polos/core/window_system.h"

#include "renderer.h"

namespace polos
{
    Renderer* Renderer::m_Instance;

    void Renderer::Startup()
    {
        m_Instance = this;

        auto win_props = WindowSystem::GetAppWindowProps();
        float aspect = static_cast<float>(win_props->width) / static_cast<float>(win_props->height);

        m_ViewMatrix = glm::lookAt(
            glm::vec3(0.0f, 0.0f, 2.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, -1.0f, 0.0f)
        );

        m_ProjectionMatrix = glm::ortho(
            -aspect,
            aspect,
            -1.0f,
            1.0f,
            0.1f,
            100.0f
        );
    }
    
    void Renderer::Shutdown()
    {
        m_Instance = nullptr;
    }

    glm::mat4 const& Renderer::GetProjectionMatrix()
    {
        return m_ProjectionMatrix;
    }

    glm::mat4 const& Renderer::GetViewMatrix()
    {
        return m_ViewMatrix;
    }

    void RenderTexture2D(
        glm::vec2 position,
        glm::vec2 rotation,
        glm::mat4& model,
        TextureRef texture,
        StringId const shader_name
    )
    {

    }
}