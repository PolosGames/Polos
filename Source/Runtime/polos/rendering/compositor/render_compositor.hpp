///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_RENDER_COMPOSITOR_HPP
#define POLOS_RENDERING_RENDER_COMPOSITOR_HPP

#include "polos/rendering/compositor/frame_targets.hpp"
// #include "polos/rendering/passes/opaque_geometry_pass.hpp"
#include "polos/rendering/passes/forward_shading_pass.hpp"
#include "polos/rendering/passes/ui_render_pass.hpp"
#include "polos/rendering/settings.hpp"
#include "polos/rendering/uniforms/camera_uniform.hpp"
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
struct Shader;
class RenderContext;
class Buffer;

class RenderCompositor
{
public:
    struct CreationDetails
    {
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
    auto Record(FrameData const& tFrameData) -> int32_t;
private:
    auto createTargets() -> Result<void>;
    auto releaseTargets() -> void;
    auto createColorTarget() -> bool;
    auto createDepthTarget() -> bool;

    std::array<FrameTargets, Settings::kMaxFramesInFlight> mFrameTargets;
    std::shared_ptr<Shader const>                          mQuadVertexShader;
    std::shared_ptr<Shader const>                          mQuadFragmentShader;
    std::shared_ptr<Shader const>                          mForwardVertexShader;
    std::shared_ptr<Shader const>                          mForwardFragmentShader;
    std::shared_ptr<Shader const>                          mMainComputeShader;

    std::unique_ptr<Buffer> mInstanceBuffer;

    //std::unique_ptr<OpaqueGeometryPass>                    mOpaqueGeometryPass;
    std::unique_ptr<ForwardShadingPass> mForwardShadingPass;
    std::unique_ptr<UiRenderPass>       mUiRenderPass;

    CameraUniform mCameraUbo;
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_RENDER_COMPOSITOR_HPP
