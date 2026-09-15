///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SRC_RESOURCES_BUFFER_DESCRIPTION_HPP
#define POLOS_RENDERING_SRC_RESOURCES_BUFFER_DESCRIPTION_HPP

#include "resources/allocation_params.hpp"

#include <vulkan/vulkan.h>

namespace polos::rendering
{

struct BufferDescription
{
    VkDeviceSize       size{0U};
    VkBufferUsageFlags usage{0U};
    MemoryResidence    residence{MemoryResidence::kDevice};
    HostAccessFlags    hostAccess{HostAccessFlags::kNone};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_SRC_RESOURCES_BUFFER_DESCRIPTION_HPP
