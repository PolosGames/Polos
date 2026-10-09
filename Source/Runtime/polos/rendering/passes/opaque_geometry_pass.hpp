///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_OPAQUE_GEOMETRY_PASS_HPP
#define POLOS_RENDERING_OPAQUE_GEOMETRY_PASS_HPP

#include "polos/rendering/ssbo/quad_instance.hpp"
#include "polos/rendering/uniforms/camera_uniform.hpp"
#include "polos/rendering/vk/settings.hpp"
#include "polos/rendering/vk/vulkan_pipeline.hpp"
#include "polos/resource/resource_manager.hpp"

#include <vulkan/vulkan.h>

#include <array>
#include <vector>

namespace polos::rendering
{

class OpaqueGeometryPass
{
public:
    struct QuadBatch
    {
        std::vector<QuadInstance>                                quadInstances;
        std::array<resource::ResourceId, Settings::kMaxSamplers> textures;
    };

    struct Inputs
    {
        std::uint32_t frameSlot{0U};

        VkCommandBuffer   cmdBuf;
        VkRect2D const&   renderArea;
        VkExtent2D const& extent;
        VkRect2D const&   scissor;
        VkViewport const& viewport;

        CameraUniform const& cameraUniform;
        std::span<QuadBatch> quadBatches;

        VkImageView colorView;
        VkImageView depthView;

        Buffer* quadVertices;
        Buffer* quadIndices;
    };

    OpaqueGeometryPass();
    ~OpaqueGeometryPass();

    auto Prepare(std::uint32_t tFrameSlot, std::span<QuadBatch> tQuadBatches) -> void;
    auto Record(Inputs const& tInputs) -> void;

    static constexpr std::uint32_t const kDescriptorSet0Count = Settings::kMaxFramesInFlight;
    static constexpr std::uint32_t const kDescriptorSet1Count = Settings::kMaxFramesInFlight * Settings::kMaxBatches;
private:
    VkRenderPass                         mRenderPass{VK_NULL_HANDLE};
    std::array<VkDescriptorSetLayout, 2U> mSetLayouts{};
    VulkanPipeline                       mQuadPipeline;// Owned by PipelineCache.
    VkDescriptorPool                     mDescriptorPool{VK_NULL_HANDLE};

    std::array<VkDescriptorSet, kDescriptorSet0Count> mDescriptorSet0{};
    std::array<VkDescriptorSet, kDescriptorSet1Count> mDescriptorSet1{};

    std::array<VkFramebuffer, Settings::kMaxFramesInFlight> mFramebuffers{};

    std::vector<QuadInstance> mFrameInstances;
    std::array<std::unique_ptr<Buffer>, Settings::kMaxFramesInFlight> mCameraUniforms;
    std::array<std::unique_ptr<Buffer>, Settings::kMaxFramesInFlight> mInstanceBuffers;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_OPAQUE_GEOMETRY_PASS_HPP
