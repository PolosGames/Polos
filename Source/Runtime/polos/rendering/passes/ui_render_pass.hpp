///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_UI_RENDER_PASS_HPP
#define POLOS_RENDERING_UI_RENDER_PASS_HPP

#include "polos/rendering/vk/settings.hpp"

#include <vulkan/vulkan.h>

#include <array>
#include <cstdint>

namespace polos::rendering
{

class UiRenderPass
{
public:
    struct Inputs
    {
        std::uint32_t   frameSlot;
        VkCommandBuffer cmdBuf;
        VkImageView     colorView;
        VkExtent2D      extent;
    };

    UiRenderPass() = default;
    ~UiRenderPass();

    UiRenderPass(UiRenderPass const&)                    = delete;
    auto operator=(UiRenderPass const&) -> UiRenderPass& = delete;

    auto Create(VkFormat tColorFormat) -> bool;
    auto ReleaseFramebuffers() -> void;
    auto Record(Inputs const& tInputs) -> void;
private:
    VkRenderPass                                            mRenderPass{VK_NULL_HANDLE};
    std::array<VkFramebuffer, Settings::kMaxFramesInFlight> mFramebuffers{};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_UI_RENDER_PASS_HPP
