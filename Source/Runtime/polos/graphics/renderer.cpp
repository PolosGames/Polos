#include "renderer.h"

#include <glm/gtc/matrix_transform.hpp>

#include "polos/core/window_system.h"
#include "polos/core/camera.h"
#include "polos/graphics/vertex.h"
#include "polos/graphics/shader_lib.h"
#include "polos/core/scene/scene_view.h"
#include "polos/graphics/shapes/shapes2d_transform.h"
#include "polos/core/engine/engine.h"

namespace polos
{

    namespace
    {
        static constexpr std::array<vertex, 4> k_QuadVertices{
            vertex{glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(1.0f), glm::vec2(0.0f, 0.0f), glm::vec3{97.0f, 40.0f, 36.0f}}, // Bottom-left
            vertex{glm::vec3(0.5f,  -0.5f, 0.5f), glm::vec3(1.0f), glm::vec2(1.0f, 0.0f), glm::vec3{97.0f, 40.0f, 36.0f}}, // Bottom-right
            vertex{glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(1.0f), glm::vec2(1.0f, 1.0f), glm::vec3{97.0f, 40.0f, 36.0f}}, // Top-right
            vertex{glm::vec3(-0.5f, 0.5f,  0.5f), glm::vec3(1.0f), glm::vec2(0.0f, 1.0f), glm::vec3{97.0f, 40.0f, 36.0f}}, // Top-left
        };

        static constexpr std::array<uint32, 6> k_QuadIndices{
            0,
            1,
            2,
            2,
            3,
            0,
        };
    }

    Renderer* Renderer::s_Instance;

    Renderer::Renderer()
    {
        SUBSCRIBE_TO_ENGINE_STARTUP(Startup);
        SUBSCRIBE_TO_ENGINE_SHUTDOWN(Shutdown);
    }

    void Renderer::Startup()
    {
        s_Instance = this;
    }
    
    void Renderer::Shutdown()
    {
        s_Instance = nullptr;
    }

    void Renderer::RenderScene(Scene& scene)
    {
        for (auto [texture2d_comp, material_comp, transform_comp] : SceneView<ecs::texture2d_component, ecs::material_component, ecs::transform_component>(scene))
        {
            auto const& quad_vao       = Renderer::GetQuadVao();
            if (texture2d_comp->hasUvChanged)
            {
                std::array<vertex, 4> new_quad_vertices{
                    k_QuadVertices[0], // Bottom-left
                    k_QuadVertices[1], // Bottom-right
                    k_QuadVertices[2], // Top-right
                    k_QuadVertices[3]  // Top-left
                };

                new_quad_vertices[0].textureCoord = texture2d_comp->uvBottomLeft;
                new_quad_vertices[1].textureCoord = texture2d_comp->uvBottomRight;
                new_quad_vertices[2].textureCoord = texture2d_comp->uvTopRight;
                new_quad_vertices[3].textureCoord = texture2d_comp->uvTopLeft;

                quad_vao.SetVertexBufferData(new_quad_vertices);
            }

            auto* shader        = material_comp->shader;
            shader->Use();
            shader->SetUniform("u_Projection"_sid, Renderer::GetProjectionMatrix());
            shader->SetUniform("u_View"_sid, Camera::GetViewMatrix());

            glm::mat4 model_matrix(1.0f);
            model_matrix = glm::translate(model_matrix, transform_comp->position);
            shapes::RotateShape2D(model_matrix, transform_comp->rotation);
            model_matrix = glm::scale(model_matrix, transform_comp->scale);

            shader->SetUniform("u_Model"_sid, model_matrix);

            glBindTextureUnit(0, texture2d_comp->texture->id);
            quad_vao.Draw();
            quad_vao.Unbind();

            shader->Release();
        }
    }

    void Renderer::SetMainWindowHandle(polos::GUID handle)
    {
        s_Instance->m_MainWinHandle = handle;

        auto  win_props = WindowSystem::GetWindowProps(s_Instance->m_MainWinHandle);
        float aspect = static_cast<float>(win_props->width) / static_cast<float>(win_props->height);

        s_Instance->m_ProjectionMatrix = glm::ortho(
            -aspect, aspect,
              -1.0f,   1.0f,
               0.1f, 100.0f
        );
    }

    glm::mat4 const& Renderer::GetProjectionMatrix()
    {
        return s_Instance->m_ProjectionMatrix;
    }

    polos::Vao& Renderer::GetQuadVao()
    {
        return *s_Instance->m_QuadVao;
    }

    SharedPtr<GraphicsContext> Renderer::InitializeGfxContext()
    {
        s_Instance->m_GfxContext = std::make_shared<GraphicsContext>();
        s_Instance->m_GfxContext->Initialize(nullptr);

        s_Instance->m_QuadVao = MakeUnique<polos::Vao>(k_QuadVertices, k_QuadIndices);

        return s_Instance->m_GfxContext;
    }
}