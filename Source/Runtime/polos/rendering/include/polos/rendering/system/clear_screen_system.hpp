//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_SYSTEM_CLEAR_SCREEN_SYSTEM_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_SYSTEM_CLEAR_SCREEN_SYSTEM_HPP_

#include "polos/rendering/i_render_system.hpp"
#include "polos/rendering/module_macros.hpp"

namespace polos::rendering
{

class RENDERING_EXPORT ClearScreenSystem : public IRenderSystem
{
public:
    ClearScreenSystem(RenderContext& t_context, RenderGraph& t_graph);

    void Initialize() override;
    void Update() override;
private:
    RenderGraphResourceHandle m_sc_img_handle;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_SYSTEM_CLEAR_SCREEN_SYSTEM_HPP_
