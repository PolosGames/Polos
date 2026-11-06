///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_GUI_BACKEND_HPP
#define POLOS_GUI_BACKEND_HPP

#include "polos/utils/string_id.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>

namespace polos::gui::backend
{
auto Initialize(VkRenderPass tRenderPass) -> bool;
auto Shutdown() -> void;
auto BeginFrame() -> void;
auto RecordDrawData(VkCommandBuffer tCommandBuffer) -> void;
auto GetImageTexture(utils::string_id tTexture) -> std::uint64_t;
}// namespace polos::gui::backend

#endif// POLOS_GUI_BACKEND_HPP
