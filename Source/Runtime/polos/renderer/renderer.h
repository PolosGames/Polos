#pragma once
#ifndef POLOS_RENDERER_RENDERER_H
#define POLOS_RENDERER_RENDERER_H

#include <glm/glm.hpp>

#include "polos/containers/containers.h"
#include "polos/context/texture.h"
#include "polos/context/shader.h"

namespace polos
{
    class Renderer
    {
    public:
        Renderer() = default;
        ~Renderer() = default;

        void Startup();
        void Shutdown();
        
        glm::mat4 const& GetProjectionMatrix();
        glm::mat4 const& GetViewMatrix();

    private:
        static Renderer* m_Instance;

        glm::mat4 m_ViewMatrix;
        glm::mat4 m_ProjectionMatrix;
    };

    void RenderTexture2D(
        glm::vec2 position,
        glm::vec2 rotation,
        glm::mat4& model,
        TextureRef texture,
        StringId const shader_name
    );
}

#endif /* POLOS_RENDERER_RENDERER_H */
