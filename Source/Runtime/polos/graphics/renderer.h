#pragma once

#include <glm/glm.hpp>

#include "polos/containers/containers.h"
#include "polos/graphics/texture.h"
#include "polos/graphics/shader.h"
#include "polos/utils/guid.h"
#include "polos/graphics/vao.h"

namespace polos
{
    class Renderer
    {
    public:
        void Startup();
        void Shutdown();

        static void SetMainWindowHandle(polos::GUID p_Handle);
        
        static glm::mat4 const& GetProjectionMatrix();
        static polos::Vao& GetRectangleVao();

    private:
        static Renderer* s_Instance;

        polos::GUID  m_MainWinHandle;
        polos::Vao   m_RectangleVao;
        glm::mat4    m_ProjectionMatrix;
    };

    void RenderRectangle(
        glm::mat4& model_matrix,
        Shader& shader
    );

    void RenderTexture2D(
        glm::mat4& model_matrix,
        TextureRef texture,
        Shader& shader
    );
}
