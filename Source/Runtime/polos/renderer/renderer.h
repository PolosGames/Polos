#pragma once
#ifndef POLOS_RENDERER_RENDERER_H
#define POLOS_RENDERER_RENDERER_H

#include <glm/glm.hpp>

#include "polos/containers/containers.h"
#include "polos/context/texture.h"
#include "polos/context/shader.h"
#include "polos/utils/guid.h"
#include "polos/context/vao.h"

namespace polos
{
    class Renderer
    {
    public:
        void Startup();
        void Shutdown();

        static void SetMainWindowHandle(pl::GUID handle);
        
        static glm::mat4 const& GetProjectionMatrix();
        static pl::Vao& GetRectangleVao();

    private:
        static Renderer* m_Instance;

        pl::GUID  m_MainWinHandle;
        pl::Vao   m_RectangleVao;
        glm::mat4 m_ProjectionMatrix;
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

#endif /* POLOS_RENDERER_RENDERER_H */
