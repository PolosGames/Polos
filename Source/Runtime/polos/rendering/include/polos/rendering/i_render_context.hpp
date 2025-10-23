//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_I_RENDER_CONTEXT_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_I_RENDER_CONTEXT_HPP_

#include "polos/rendering/common.hpp"
#include "polos/rendering/texture_2d.hpp"

#include "polos/rendering/module_macros.hpp"

namespace polos::rendering
{

class IRenderGraph;

class IRenderContext
{
public:
    virtual ~IRenderContext() = default;

    virtual Result<void> Initialize(GLFWwindow* t_window) = 0;
    virtual Result<void> Shutdown()                       = 0;

    virtual VkCommandBuffer BeginFrame()           = 0;
    virtual void            EndFrame()             = 0;
    virtual IRenderGraph&   GetRenderGraph() const = 0;
    virtual bool            IsInitialized() const  = 0;
};

}// namespace polos::rendering

extern "C"
{
    [[nodiscard]] RENDERING_EXPORT polos::rendering::IRenderContext* CreateRenderContext();
}

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_I_RENDER_CONTEXT_HPP_
