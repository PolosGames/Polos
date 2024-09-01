#include "renderer.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "polos/core/ecs/sets/camera_set.h"
#include "polos/core/engine/engine.h"
#include "polos/core/scene/common_set_scene_view.h"
#include "polos/core/scene/component_scene_view.h"
#include "polos/core/window_system.h"
#include "polos/graphics/shader_lib.h"
#include "polos/graphics/shapes/shapes2d_transform.h"
#include "polos/graphics/quad_vertex.h"

namespace polos
{

namespace
{
    static constexpr std::array<uint32, 6> k_QuadIndices{
        0, 1, 2, 2, 3, 0,
    };
}// namespace

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
    for (auto [info_component] : ComponentSceneView<ecs::info_component>(scene)) { PL_VOID_CAST(info_component); }

    for (auto camera_set : CommonSetSceneView<ecs::camera_set>(scene))
    {
        for (auto const& shader : ShaderLib::GetAll())
        {
            shader.Use();
            shader.SetUniform("u_View"_sid, camera_set.GetViewMatrix());
            shader.Release();
        }
    }

    for (auto [transform_comp] : ComponentSceneView<ecs::transform_component>(scene)) { PL_VOID_CAST(transform_comp); }
}

void Renderer::SetMainWindowHandle(polos::GUID handle)
{
    s_Instance->m_MainWinHandle = handle;

    auto  win_props = WindowSystem::GetWindowProps(s_Instance->m_MainWinHandle);
    float aspect    = static_cast<float>(win_props->width) / static_cast<float>(win_props->height);

    s_Instance->m_ProjectionMatrix = glm::perspective(60.0f, aspect, .1f, 1000.f);
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
}// namespace polos