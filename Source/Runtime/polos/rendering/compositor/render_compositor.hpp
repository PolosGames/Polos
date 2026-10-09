///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_RENDER_COMPOSITOR_HPP
#define POLOS_RENDERING_RENDER_COMPOSITOR_HPP

#include "polos/rendering/compositor/frame_targets.hpp"
#include "polos/rendering/passes/opaque_geometry_pass.hpp"
#include "polos/rendering/vk/settings.hpp"
#include "polos/utils/error_code.hpp"

#include <vulkan/vulkan.h>

#include <array>
#include <memory>
#include <vector>

namespace polos::ecs
{
struct SceneData;
}// namespace polos::ecs

namespace polos::rendering
{

struct FrameData;
class RenderContext;
class Buffer;

class RenderCompositor
{
public:
    struct CreationDetails
    {
        RenderContext& context;
    };

    RenderCompositor()  = default;
    ~RenderCompositor() = default;

    RenderCompositor(RenderCompositor const&);
    RenderCompositor(RenderCompositor&&);
    RenderCompositor& operator=(RenderCompositor const&);
    RenderCompositor& operator=(RenderCompositor&&);

    auto Create(CreationDetails const& tDetails) -> Result<void>;
    auto Destroy() -> void;
    auto RecreateTargets() -> Result<void>;

    auto Prepare(FrameData const& tFrameData) -> void;
    auto Record(FrameData const& tFrameData) -> void;
private:
    auto createTargets() -> Result<void>;
    auto releaseTargets() -> void;
    auto createColorTarget() -> bool;
    auto createDepthTarget() -> bool;

    std::array<FrameTargets, Settings::kMaxFramesInFlight> mFrameTargets;
    std::unique_ptr<OpaqueGeometryPass>                    mOpaqueGeometryPass;

    std::vector<OpaqueGeometryPass::QuadBatch> mQuadBatches;
    CameraUniform                              mCameraUbo;

    std::unique_ptr<Buffer> mQuadVertexBuffer;
    std::unique_ptr<Buffer> mQuadIndexBuffer;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_RENDER_COMPOSITOR_HPP
