///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_SRC_FRAME_PIPELINE_HPP
#define POLOS_RENDERING_SRC_FRAME_PIPELINE_HPP

#include "compositor/frame_targets.hpp"
#include "polos/communication/error_code.hpp"

#include <vulkan/vulkan.h>

#include <memory>

namespace polos::rendering
{

class GeneralPass;
struct FrameData;
struct RenderContext;
struct SceneData;

class RenderCompositor
{
public:
    explicit RenderCompositor(RenderContext& tRenderContext);

    RenderCompositor(RenderCompositor const&);
    RenderCompositor(RenderCompositor&&);
    RenderCompositor& operator=(RenderCompositor const&);
    RenderCompositor& operator=(RenderCompositor&&);
    ~RenderCompositor();

    auto Destroy() -> void;

    auto Prepare(FrameData const& tFrameData) -> Result<void>;
    auto Record(FrameData const& tFrameData, SceneData const& tSceneData) -> void;
private:
    auto createTargets() -> void;
    auto releaseTargets() -> void;

    auto createColorTarget() -> void;
    auto createDepthTarget() -> void;

    VkExtent2D                   mLastExtent;
    std::unique_ptr<GeneralPass> mGeneralPass;

    FrameTargets mFrameTargets;

    VkDevice mDevice;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_SRC_FRAME_PIPELINE_HPP
