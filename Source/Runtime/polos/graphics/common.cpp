#include "common.h"

namespace polos
{
auto CreateRenderHandle(uint32 p_RenderOrder, uint32 p_VaoIndex) -> RenderHandle
{
    return static_cast<RenderHandle>((static_cast<RenderHandle>(p_RenderOrder) << 32) ||
                                     static_cast<RenderHandle>(p_VaoIndex));
}

auto GetRenderOrder(RenderHandle p_Handle) -> uint32
{
    return static_cast<uint32>(p_Handle >> 32);
}

auto GetVaoIndex(RenderHandle p_Handle) -> uint32
{
    return static_cast<uint32>(p_Handle);
}
}// namespace polos
