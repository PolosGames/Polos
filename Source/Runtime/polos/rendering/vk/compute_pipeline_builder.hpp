///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_COMPUTE_PIPELINE_BUILDER_HPP
#define POLOS_RENDERING_COMPUTE_PIPELINE_BUILDER_HPP

#include "polos/polos_api.hpp"
#include "polos/rendering/vk/vulkan_pipeline.hpp"

#include <vulkan/vulkan.h>

#include <cstddef>
#include <memory>
#include <span>
#include <vector>

namespace polos::rendering
{

struct Shader;

class POLOS_API ComputePipelineBuilder
{
public:
    ComputePipelineBuilder()                              = default;
    ComputePipelineBuilder(ComputePipelineBuilder const&) = delete;
    ComputePipelineBuilder(ComputePipelineBuilder&&)      = default;
    ~ComputePipelineBuilder()                             = default;

    auto operator=(ComputePipelineBuilder const&) -> ComputePipelineBuilder& = delete;
    auto operator=(ComputePipelineBuilder&&) -> ComputePipelineBuilder&      = default;

    void SetShader(std::shared_ptr<Shader const> tShader);
    void SetFlags(VkPipelineCreateFlags tFlags);
    void AddDescriptorSetLayout(VkDescriptorSetLayout tLayout);
    void AddPushConstantRange(VkPushConstantRange tRange);
    void
    SetSpecializationConstants(std::span<VkSpecializationMapEntry const> tEntries, std::span<std::byte const> tData);

    [[nodiscard]] auto Finalize() const -> VulkanPipeline;
private:
    std::shared_ptr<Shader const>         mShader;
    VkPipelineCreateFlags                 mFlags{0U};
    std::vector<VkDescriptorSetLayout>    mLayouts;
    std::vector<VkPushConstantRange>      mPushConstants;
    std::vector<VkSpecializationMapEntry> mSpecializationEntries;
    std::vector<std::byte>                mSpecializationData;
};

}// namespace polos::rendering

#endif
