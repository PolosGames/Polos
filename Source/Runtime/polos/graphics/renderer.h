#pragma once

#include <glm/glm.hpp>

#include "polos/containers/containers.h"
#include "polos/graphics/texture.h"
#include "polos/graphics/shader.h"
#include "polos/utils/guid.h"
#include "polos/graphics/vao.h"
#include "polos/graphics/graphics_context.h"
#include "polos/core/scene/scene.h"

namespace polos
{
    class Renderer
    {
    public:
        Renderer();
    public:
        void Startup();
        void Shutdown();

        static SharedPtr<GraphicsContext> InitializeGfxContext();

        static void RenderScene(Scene& scene);

        static void SetMainWindowHandle(polos::GUID p_Handle);
        
        static glm::mat4 const& GetProjectionMatrix();
        static polos::Vao&      GetQuadVao();

    private:
        static Renderer* s_Instance;

        polos::GUID                m_MainWinHandle;
        UniquePtr<polos::Vao>      m_QuadVao;
        glm::mat4                  m_ProjectionMatrix{};
        SharedPtr<GraphicsContext> m_GfxContext;
    };
}
