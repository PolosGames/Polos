///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_PIPELINE_CACHE_HPP
#define POLOS_RENDERING_PIPELINE_CACHE_HPP

#include "polos/polos_api.hpp"
#include "polos/rendering/cache/graphics_pipeline_info.hpp"
#include "polos/rendering/vk/vulkan_pipeline.hpp"
#include "polos/utils/error_code.hpp"
#include "polos/utils/string_id.hpp"

#include <vulkan/vulkan.h>

#include <unordered_map>

namespace polos::rendering
{

class VulkanSwapchain;


class POLOS_API PipelineCache
{
public:
    struct CreationDetails// NOLINT
    {
    };

    PipelineCache();
    ~PipelineCache();

    PipelineCache(PipelineCache const&)            = delete;
    PipelineCache(PipelineCache&&)                 = delete;
    PipelineCache& operator=(PipelineCache const&) = delete;
    PipelineCache& operator=(PipelineCache&&)      = delete;

    auto Create(CreationDetails const& tDetails) -> Result<void>;
    auto Destroy() -> void;

    /// @brief Gets the whole info about the pipeline from the cache in the wrapper VulkanPipeline
    /// @param tPipelineName The name of the pipeline to get in string_id.
    /// @return The pipeline entry, or VK_NULL_HANDLE for both pipeline and pipeline layout
    [[nodiscard]] auto GetPipeline(utils::string_id tPipelineName) const -> VulkanPipeline;

    /// @brief Gets the specified pipeline's VkPipeline from the cache
    /// @param tPipelineName The name of the pipeline to get in string_id.
    /// @return The VkPipeline, or VK_NULL_HANDLE
    [[nodiscard]] auto GetVkPipeline(utils::string_id tPipelineName) const -> VkPipeline;

    /// @brief Gets the specified pipeline's VkPipelineLayout from the cache
    /// @param tPipelineName The name of the pipeline to get in string_id.
    /// @return The VkPipelineLayout, or VK_NULL_HANDLE
    [[nodiscard]] auto GetVkPipelineLayout(utils::string_id tPipelineName) const -> VkPipelineLayout;

    /// @brief Gets the specified pipeline's Descriptor Layouts from the cache
    /// @param tPipelineName The name of the pipeline to get in string_id.
    /// @return The DescriptorLayouts span, or a null span
    /// @note Handles are borrowed from the caller of ConstructPipeline and require its layouts to remain alive.
    [[nodiscard]] auto GetDescriptorLayouts(utils::string_id tPipelineName) const
        -> std::span<VkDescriptorSetLayout const>;

    /// @brief Tries to load the pipeline from cache disk
    /// @todo We should have no need for this function after finalizing what pipelines we'll need.
    /// @param tPipelineInfo The information to create the pipeline with.
    /// @return The pipeline, or a value with pipeline == VK_NULL_HANDLE on failure.
    /// @note Descriptor set layouts remain caller-owned. Reusing a name requires compatible pipeline settings.
    [[nodiscard]] auto ConstructPipeline(GraphicsPipelineInfo const& tPipelineInfo) -> VulkanPipeline;
private:
    struct CacheEntry
    {
        VkPipeline                         pipeline;
        VkPipelineLayout                   pipelineLayout;
        std::vector<VkDescriptorSetLayout> dsLayouts;// Borrowed; owned by the rendering pass.
    };

    [[nodiscard]] auto fromCacheEntry(CacheEntry const& tEntry) const -> VulkanPipeline;

    using Cache = std::unordered_map<utils::string_id, CacheEntry>;

    VkDevice mDevice{VK_NULL_HANDLE};

    Cache           mCache;
    VkPipelineCache mCreationCache{VK_NULL_HANDLE};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_PIPELINE_CACHE_HPP
