//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDERER_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDERER_HPP_

namespace polos::rendering
{
class Renderer
{
public:
    Renderer()                      = default;
    ~Renderer()                     = default;
    Renderer(Renderer&&)            = delete;
    Renderer(Renderer&)             = delete;
    Renderer& operator=(Renderer&&) = delete;
    Renderer& operator=(Renderer&)  = delete;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_RENDERER_HPP_
