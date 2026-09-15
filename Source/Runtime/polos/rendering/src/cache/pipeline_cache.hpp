///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SRC_CACHE_PIPELINE_CACHE_HPP
#define POLOS_RENDERING_SRC_CACHE_PIPELINE_CACHE_HPP

#include "cache/graphics_pipeline_info.hpp"
#include "polos/communication/error_code.hpp"
#include "polos/polos_api.hpp"
#include "polos/utils/string_id.hpp"
#include "vk/vulkan_pipeline.hpp"

#include <vulkan/vulkan.h>

#include <unordered_map>

namespace polos::rendering
{

class VulkanSwapchain;

struct alignas(16) PipelineCacheCreateDetails// NOLINT
{
    VkDevice         logiDevice{VK_NULL_HANDLE};
    VulkanSwapchain* swapchain{nullptr};
};

class POLOS_API PipelineCache
{
public:
    PipelineCache();
    ~PipelineCache();

    PipelineCache(PipelineCache const&)            = delete;
    PipelineCache(PipelineCache&&)                 = delete;
    PipelineCache& operator=(PipelineCache const&) = delete;
    PipelineCache& operator=(PipelineCache&&)      = delete;

    auto Create(PipelineCacheCreateDetails const& tDetails) -> Result<void>;
    auto Destroy() -> Result<void>;

    /// @brief Gets a pipeline from the cache
    /// @param tPipelineName The name of the pipeline to get in string_id.
    auto GetPipeline(utils::string_id tPipelineName) const -> Result<VulkanPipeline>;

    /// @brief Gets a pipeline from the cache
    /// @param tPipelineName The name of the pipeline to get.
    auto GetPipeline(std::string_view tPipelineName) const -> Result<VulkanPipeline>;

    /// @brief Tries to load the pipeline from cache disk
    /// @todo We should have no need for this function after finalizing what pipelines we'll need.
    /// @param tPipelineInfo The information to create the pipeline with.
    auto ConstructPipeline(GraphicsPipelineInfo const& tPipelineInfo) -> Result<VulkanPipeline>;
private:
    VkDevice mDevice{VK_NULL_HANDLE};

    VulkanSwapchain* mSwapchain{nullptr};

    std::unordered_map<utils::string_id, VulkanPipeline> mCache;
    VkPipelineCache                                      mCreationCache{VK_NULL_HANDLE};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_SRC_CACHE_PIPELINE_CACHE_HPP
