///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SRC_FRAME_PIPELINE_HPP
#define POLOS_RENDERING_SRC_FRAME_PIPELINE_HPP

#include "compositor/frame_targets.hpp"
#include "passes/general_pass.hpp"
#include "polos/communication/error_code.hpp"

#include <vulkan/vulkan.h>

#include <memory>

namespace polos::rendering
{

struct FrameData;
class RenderContext;
struct SceneData;

struct RenderCompositorCreateDetails
{
    RenderContext& context;
    VkExtent3D     scExtent;
    VkDevice       logiDevice;
};

class RenderCompositor
{
public:
    RenderCompositor() = default;

    RenderCompositor(RenderCompositor const&);
    RenderCompositor(RenderCompositor&&);
    RenderCompositor& operator=(RenderCompositor const&);
    RenderCompositor& operator=(RenderCompositor&&);
    ~RenderCompositor();

    auto Create(RenderCompositorCreateDetails const& tDetails) -> Result<void>;
    auto Destroy() -> void;

    auto Prepare(FrameData const& tFrameData) -> Result<void>;
    auto Record(FrameData const& tFrameData, SceneData const& tSceneData) -> void;
private:
    auto createTargets() -> Result<void>;
    auto releaseTargets() -> void;

    auto createColorTarget() -> bool;
    auto createDepthTarget() -> bool;

    VkExtent3D                   mLastExtent{};
    std::unique_ptr<GeneralPass> mGeneralPass;

    std::array<FrameTargets, Settings::kMaxFramesInFlight> mFrameTargets;

    VkDevice mDevice{VK_NULL_HANDLE};
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_SRC_FRAME_PIPELINE_HPP
