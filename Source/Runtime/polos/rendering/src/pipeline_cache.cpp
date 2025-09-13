//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#include "polos/rendering/pipeline_cache.hpp"
#include "polos/filesystem/file_manip.hpp"
#include "polos/logging/log_macros.hpp"
#include "polos/rendering/graphics_pipeline_info.hpp"
#include <vulkan/vulkan_core.h>

namespace polos::rendering
{

auto PipelineCache::GetPipeline(graphics_pipeline_info const& t_pipeline_info) -> VkPipeline
{
    auto pipeline_key = std::hash<graphics_pipeline_info>{}(t_pipeline_info);

    auto it = m_cache.find(pipeline_key);
    if (it == m_cache.end())
    {
        LogWarn("Requested pipeline was not found in pipeline cache. Create it with LoadOrCreatePipeline!");
        return VK_NULL_HANDLE;
    };

    return it->second;
}

auto PipelineCache::LoadOrCreatePipeline(graphics_pipeline_info const& t_pipeline_info) -> VkPipeline
{
    VkPipelineCacheCreateInfo info{
        .sType           = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
        .pNext           = nullptr,
        .flags           = 0U,
        .initialDataSize = 0U,
        .pInitialData    = nullptr,
    };

    auto res = fs::ReadFile(std::filesystem::path("Resource/Cache/pipeline.dat"));
    if (!res.has_value())
    {
        LogWarn("{}", res.error());
    }

    vkCreatePipelineCache(m_device, info, nullptr, &m_creation_cache);
}

}// namespace polos::rendering
