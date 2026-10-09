///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_GENERAL_PASS_HPP
#define POLOS_RENDERING_GENERAL_PASS_HPP

#include "polos/ecs/scene_data.hpp"
#include "polos/rendering/vk/settings.hpp"
#include "polos/resource/model.hpp"
#include "polos/utils/error_code.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace polos::rendering
{

struct FrameData;
struct FrameTargets;
class Buffer;
class Image;
class RenderContext;
class VulkanSwapchain;
class ShaderCache;
class PipelineCache;

struct GeneralPassInputs
{
};

class GeneralPass
{
public:
    explicit GeneralPass(RenderContext& tContext);
    ~GeneralPass();

    auto Initialize() -> Result<void>;
    auto Prepare() -> void;
    auto Record(FrameData const& tFrameData, FrameTargets const& tTargets) -> void;
private:
    VkRenderPass       createRenderPass();
    VkFramebuffer      createFramebuffer();
    VkPipeline         createPipeline();
    [[nodiscard]] auto createVertexBuffer() -> bool;
    [[nodiscard]] auto createIndexBuffer() -> bool;
    auto               createUboMapping() -> Result<void>;
    VkDescriptorPool   createDescriptorPool();
    auto               createDescriptorSets() -> void;
    VkImageView        createTexture();
    VkSampler          createTextureSampler();

    RenderContext&   mContext;
    VulkanSwapchain* mSwapchain{nullptr};
    ShaderCache*     mShaderCache{nullptr};
    PipelineCache*   mPipelineCache{nullptr};

    VkCommandBuffer mCommandBuffer{VK_NULL_HANDLE};

    VkPipeline                                                        mPipeline{VK_NULL_HANDLE};
    VkDevice                                                          mDevice{VK_NULL_HANDLE};
    std::array<VkFramebuffer, Settings::kMaxFramesInFlight>           mPassFb{};
    std::unique_ptr<Buffer>                                           mVerticesBuffer;
    std::unique_ptr<Buffer>                                           mIndicesBuffer;
    std::array<std::unique_ptr<Buffer>, Settings::kMaxFramesInFlight> mInstanceBuffers;
    std::array<void*, Settings::kMaxFramesInFlight>                   mInstanceMappings;
    std::vector<polos::resource::model::Vertex>                       mVertices;
    std::vector<std::uint16_t>                                        mIndices;
    std::array<std::unique_ptr<Buffer>, Settings::kMaxFramesInFlight> mUboBuffers;
    std::array<void*, Settings::kMaxFramesInFlight>                   mUboMappings;
    std::unique_ptr<Image>                                            mTuxImage;
    VkImageView                                                       mImageViewTuxTexture{VK_NULL_HANDLE};
    VkSampler                                                         mSamplerTuxTexture{VK_NULL_HANDLE};

    std::array<VkClearValue, 2U> mClearVals;

    VkDescriptorPool                                          mDescriptorPool{VK_NULL_HANDLE};
    std::array<VkDescriptorSet, Settings::kMaxFramesInFlight> mDescriptorSets;
    std::vector<VkDescriptorSetLayout>                        mDescriptorSetLayouts;

    VkRenderPass mRenderPass{VK_NULL_HANDLE};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_GENERAL_PASS_HPP
