#pragma once

namespace polos
{
auto CreateRenderHandle(uint32 p_RenderOrder, uint32 p_VaoIndex) -> RenderHandle;
auto GetRenderOrder(RenderHandle p_Handle) -> uint32;
auto GetVaoIndex(RenderHandle p_Handle) -> uint32;
}// namespace polos
