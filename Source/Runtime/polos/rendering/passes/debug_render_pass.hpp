///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_DEBUG_RENDER_PASS_HPP
#define POLOS_RENDERING_DEBUG_RENDER_PASS_HPP

#include "polos/rendering/debug_draw.hpp"
#include "polos/rendering/resources/buffer.hpp"
#include "polos/rendering/resources/shader.hpp"
#include "polos/rendering/settings.hpp"
#include "polos/rendering/vk/descriptor_pool.hpp"
#include "polos/rendering/vk/vulkan_pipeline.hpp"
#include "polos/rendering/vk/vulkan_render_pass.hpp"

#include <array>
#include <memory>
#include <span>

namespace polos::rendering
{
class CommandBuffer;
struct FrameTargets;

class DebugRenderPass
{
public:
    struct Inputs
    {
        uint32_t          frameSlot;
        CommandBuffer&    cmdBuf;
        VkRect2D const&   renderArea;
        VkRect2D const&   scissor;
        VkViewport const& viewport;
    };

    DebugRenderPass(
        std::shared_ptr<Shader const> tVertexShader,
        std::shared_ptr<Shader const> tFragmentShader,
        std::span<Buffer*>            tCameraUniforms);
    ~DebugRenderPass();

    auto CreateFramebuffers(std::span<FrameTargets const> tTargets, VkExtent2D const& tExtent) -> void;
    auto ReleaseFramebuffers() -> void;

    auto Prepare(uint32_t tFrameSlot, DebugDrawQueue const& tQueue) -> void;
    auto Record(Inputs const& tInputs) -> int32_t;
private:
    VulkanRenderPass                                        mRenderPass;
    std::array<VkFramebuffer, Settings::kMaxFramesInFlight> mFramebuffers{};

    VulkanPipeline mDebugPipeline;
    DescriptorPool mDescPool;

    struct WireMesh
    {
        std::unique_ptr<Buffer> vertices;
        std::unique_ptr<Buffer> indices;
        uint32_t                indexCount{0U};
    };

    struct InstanceBatch
    {
        std::unique_ptr<Buffer> buffer;
        uint32_t                count{0U};
    };

    std::array<WireMesh, kDebugPrimitiveCount>                                                mMeshes;
    std::array<std::array<InstanceBatch, kDebugPrimitiveCount>, Settings::kMaxFramesInFlight> mInstanceBatches;
};
}// namespace polos::rendering

#endif// POLOS_RENDERING_DEBUG_RENDER_PASS_HPP
