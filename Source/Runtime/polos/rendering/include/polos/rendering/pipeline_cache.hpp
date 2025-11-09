//
// Copyright (c) 2025 Kayra Urfali
// Permission is hereby granted under the MIT License - see LICENSE for details.
//

#ifndef POLOS_RENDERING_INCLUDE_POLOS_RENDERING_PIPELINE_CACHE_HPP_
#define POLOS_RENDERING_INCLUDE_POLOS_RENDERING_PIPELINE_CACHE_HPP_

#include "polos/rendering/common.hpp"
#include "polos/rendering/graphics_pipeline_info.hpp"
#include "polos/rendering/vulkan_pipeline.hpp"
#include "polos/utils/string_id.hpp"

#include <unordered_map>

namespace polos::rendering
{

class VulkanSwapchain;

struct pipeline_cache_create_details
{
    VkDevice         logi_device{VK_NULL_HANDLE};
    VulkanSwapchain* swapchain{nullptr};
};

class PipelineCache
{
public:
    PipelineCache();
    ~PipelineCache();

    PipelineCache(PipelineCache const&)            = delete;
    PipelineCache(PipelineCache&&)                 = delete;
    PipelineCache& operator=(PipelineCache const&) = delete;
    PipelineCache& operator=(PipelineCache&&)      = delete;

    auto Create(pipeline_cache_create_details const& t_details) -> Result<void>;
    auto Destroy() -> Result<void>;

    /// @brief Gets a pipeline from the cache
    /// @param t_pipeline_name The name of the pipeline to get in string_id.
    auto GetPipeline(utils::string_id const t_pipeline_name) const -> Result<vulkan_pipeline>;

    /// @brief Gets a pipeline from the cache
    /// @param t_pipeline_name The name of the pipeline to get.
    auto GetPipeline(std::string_view const t_pipeline_name) const -> Result<vulkan_pipeline>;

    /// @brief Tries to load the pipeline from cache disk
    /// @param t_pipeline_info The information to create the pipeline with.
    auto LoadOrCreatePipeline(graphics_pipeline_info const& t_pipeline_info) -> Result<vulkan_pipeline>;
private:
    VkDevice m_device{VK_NULL_HANDLE};

    VulkanSwapchain* m_swapchain{nullptr};

    std::unordered_map<utils::string_id, vulkan_pipeline> m_cache;
    VkPipelineCache                                       m_creation_cache{VK_NULL_HANDLE};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_INCLUDE_POLOS_RENDERING_PIPELINE_CACHE_HPP_
