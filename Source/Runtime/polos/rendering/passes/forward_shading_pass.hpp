///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_PASSES_FORWARD_SHADING_PASS_HPP
#define POLOS_RENDERING_PASSES_FORWARD_SHADING_PASS_HPP

#include "polos/rendering/resources/buffer.hpp"
#include "polos/rendering/resources/image.hpp"
#include "polos/rendering/settings.hpp"
#include "polos/rendering/uniforms/camera_uniform.hpp"
#include "polos/rendering/vk/dependency_info_builder.hpp"
#include "polos/rendering/vk/descriptor_pool.hpp"
#include "polos/rendering/vk/vulkan_pipeline.hpp"
#include "polos/rendering/vk/vulkan_render_pass.hpp"

#include <vulkan/vulkan.h>

#include <array>
#include <memory>

namespace polos::rendering
{

struct FrameTargets;
struct Shader;
class ForwardShadingPass
{
public:
    struct Inputs
    {
        uint32_t        frameSlot;
        CommandBuffer& cmdBuf;

        VkRect2D const&   renderArea;
        VkExtent2D const& extent;
        VkRect2D const&   scissor;
        VkViewport const& viewport;

        CameraUniform const& cameraUbo;

        uint32_t slotCount{0U};
    };

    ForwardShadingPass(
        std::shared_ptr<Shader const> tVertexShader,
        std::shared_ptr<Shader const> tFragmentShader,
        std::shared_ptr<Shader const> tMainCompShader);
    ~ForwardShadingPass();

    auto CreateFramebuffers(std::span<FrameTargets const> tTargets, VkExtent2D const& tScExtent) -> void;
    auto ReleaseFramebuffers() -> void;

    auto Prepare(uint32_t tFrameSlot, VkBuffer tInstanceBuffer) -> void;
    auto Record(Inputs const& tInputs) -> int32_t;
private:
    VulkanRenderPass                                        mRenderPass;
    std::array<VkFramebuffer, Settings::kMaxFramesInFlight> mFramebuffers{};
    DescriptorPool                                          mDescPool;

    struct ComputeDependencies
    {
        DependencyInfo beforeDispatch;
        DependencyInfo afterDispatch;
    };

    std::array<ComputeDependencies, Settings::kMaxFramesInFlight> mComputeDependencies;

    VulkanPipeline mComputeMainPipeline{};
    VulkanPipeline mPbrPipeline{};

    std::array<std::unique_ptr<Buffer>, Settings::kMaxFramesInFlight> mCameraUniforms;

    std::unique_ptr<Buffer> mVisibleMatrixIndicesBuffer;
    std::unique_ptr<Buffer> mWorldMatrices;
};
}// namespace polos::rendering

#endif// POLOS_RENDERING_PASSES_FORWARD_SHADING_PASS_HPP
