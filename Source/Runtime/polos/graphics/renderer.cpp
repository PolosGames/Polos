#include "renderer.h"

#include <glm/gtc/matrix_transform.hpp>

#include "polos/core/window_system.h"
#include "polos/core/camera.h"
#include "polos/graphics/vertex.h"
#include "polos/graphics/shader_lib.h"


namespace polos
{
    Renderer* Renderer::s_Instance;

    // 2D Texture Rectangle

    namespace
    {
        static constexpr std::array<vertex, 5> vertices{
            vertex{glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(1.0f), glm::vec2(0.0f, 0.0f), glm::vec3{97.0f, 40.0f, 36.0f}}, // Bottom-left
            vertex{glm::vec3(0.5f,  -0.5f, 0.5f), glm::vec3(1.0f), glm::vec2(1.0f, 0.0f), glm::vec3{97.0f, 40.0f, 36.0f}}, // Bottom-right
            vertex{glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(1.0f), glm::vec2(1.0f, 1.0f), glm::vec3{97.0f, 40.0f, 36.0f}}, // Top-right
            vertex{glm::vec3(-0.5f, 0.5f,  0.5f), glm::vec3(1.0f), glm::vec2(0.0f, 1.0f), glm::vec3{97.0f, 40.0f, 36.0f}}, // Top-left
        };

        static constexpr std::array<uint32, 6> indices{
            0,
            1,
            2,
            2,
            3,
            0,
        };
    }// namespace

    // !2D Texture Rectangle

    void Renderer::Startup()
    {
        s_Instance = this;
    }
    
    void Renderer::Shutdown()
    {
        s_Instance = nullptr;
    }

    void Renderer::SetMainWindowHandle(polos::GUID handle)
    {
        auto& win_handle = s_Instance->m_MainWinHandle;
        auto& proj_matrix = s_Instance->m_ProjectionMatrix;

        win_handle = handle;

        auto win_props = WindowSystem::GetWindowProps(win_handle);
        float aspect = static_cast<float>(win_props->width) / static_cast<float>(win_props->height);

        proj_matrix = glm::ortho(
            -aspect,
            aspect,
            -1.0f,
            1.0f,
            0.1f,
            100.0f
        );

        s_Instance->m_RectangleVao = Vao{vertices, indices};
    }

    glm::mat4 const& Renderer::GetProjectionMatrix()
    {
        return s_Instance->m_ProjectionMatrix;
    }

    pl::Vao& Renderer::GetRectangleVao()
    {
        return s_Instance->m_RectangleVao;
    }

    void RenderRectangle(
        glm::mat4& model_matrix,
        Shader& shader
    )
    {
        auto& vao = Renderer::GetRectangleVao();

        auto const& l_Shader = shader;

        auto const& l_projection_matrix = Renderer::GetProjectionMatrix();
        auto const l_view_matrix = Camera::GetViewMatrix();

        l_Shader.Use();
        l_Shader.SetUniform("u_Projection"_sid, l_projection_matrix);
        l_Shader.SetUniform("u_View"_sid, l_view_matrix);
        l_Shader.SetUniform("u_Model"_sid, model_matrix);

        vao.Draw();
        vao.Unbind();

        l_Shader.Release();
    }

    void RenderTexture2D(
        glm::mat4& model_matrix,
        TextureRef texture,
        Shader& shader
    )
    {
        auto& vao = Renderer::GetRectangleVao();

        auto const& l_Shader = shader;

        auto const& l_projection_matrix = Renderer::GetProjectionMatrix();
        auto const l_view_matrix = Camera::GetViewMatrix();

        l_Shader.Use();
        l_Shader.SetUniform("u_Projection"_sid, l_projection_matrix);
        l_Shader.SetUniform("u_View"_sid, l_view_matrix);
        l_Shader.SetUniform("u_Model"_sid, model_matrix);

        glBindTextureUnit(0, texture->id);
        vao.Draw();
        vao.Unbind();

        l_Shader.Release();
    }
}