//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_I_RENDER_SYSTEM_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_I_RENDER_SYSTEM_HPP_


namespace polos::rendering
{

class RenderGraphRegistry;

class IRenderSystem
{
public:
    virtual ~IRenderSystem() = default;

    virtual void Update(RenderGraphRegistry& t_registry) = 0;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_I_RENDER_SYSTEM_HPP_
