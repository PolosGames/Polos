///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SRC_PASSES_GENERAL_PASS_HPP
#define POLOS_RENDERING_SRC_PASSES_GENERAL_PASS_HPP

#include "resources/vertex.hpp"
#include "scene/scene_data.hpp"

#include <vulkan/vulkan.h>

#include <cstdint>
#include <vector>

namespace polos::rendering
{

struct FrameData;
struct FrameTargets;
class RenderContext;
class VulkanSwapchain;
class VulkanResourceManager;
class ShaderCache;
class PipelineCache;

class GeneralPass
{
public:
    explicit GeneralPass(RenderContext& tContext);
    ~GeneralPass();

    auto Prepare() -> void;
    auto Record(FrameData const& tFrameData, SceneData const& tSceneData, FrameTargets const& tTargets) -> void;
private:
    VkRenderPass     createRenderPass();
    VkFramebuffer    createFramebuffer();
    VkPipeline       createPipeline();
    VkBuffer         createVertexBuffer();
    VkBuffer         createIndexBuffer();
    void             createUboMapping();
    VkDescriptorPool createDescriptorPool();
    void             createDescriptorSets();
    VkImageView      createTexture();
    VkSampler        createTextureSampler();

    RenderContext&         mContext;
    VulkanSwapchain*       mSwapchain{nullptr};
    VulkanResourceManager* mVrm{nullptr};
    ShaderCache*           mShaderCache{nullptr};
    PipelineCache*         mPipelineCache{nullptr};

    VkCommandBuffer mCommandBuffer{VK_NULL_HANDLE};

    VkPipeline                 mPipeline{VK_NULL_HANDLE};
    VkDevice                   mDevice{VK_NULL_HANDLE};
    VkFramebuffer              mPassFb{VK_NULL_HANDLE};
    std::int32_t               mBufferVerticesIndex{0};
    VkBuffer                   mBufferVertices{VK_NULL_HANDLE};
    std::int32_t               mBufferIndicesIndex{0};
    VkBuffer                   mBufferIndices{VK_NULL_HANDLE};
    std::vector<std::int32_t>  mBufferInstancingIndices;
    std::vector<void*>         mInstanceMappings;
    std::vector<Vertex>        mVertices;
    std::vector<std::uint16_t> mIndices;
    std::vector<std::int32_t>  mBufferIndicesUbos;
    std::vector<void*>         mUboMappings;
    std::int32_t               mTextureImageIndex{0};
    VkImageView                mImageViewTuxTexture{VK_NULL_HANDLE};
    VkSampler                  mSamplerTuxTexture{VK_NULL_HANDLE};

    std::array<VkClearValue, 2U> mClearVals;

    VkDescriptorPool                   mDescriptorPool{VK_NULL_HANDLE};
    std::vector<VkDescriptorSet>       mDescriptorSets;
    std::vector<VkDescriptorSetLayout> mDescriptorSetLayouts;

    VkRenderPass mRenderPass{VK_NULL_HANDLE};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_SRC_PASSES_GENERAL_PASS_HPP
