///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/uniforms/camera_uniform.hpp"

#include "polos/rendering/resources/buffer.hpp"
#include "polos/utils/assert.hpp"

namespace polos::rendering
{
namespace
{
auto ExtractFrustumPlanes(glm::mat4 const& tViewProj) -> std::array<glm::vec4, 6U>
{
    auto const row = [&tViewProj](glm::length_t const tRow) -> glm::vec4 {
        return glm::vec4{tViewProj[0][tRow], tViewProj[1][tRow], tViewProj[2][tRow], tViewProj[3][tRow]};
    };

    glm::vec4 const r0 = row(0);
    glm::vec4 const r1 = row(1);
    glm::vec4 const r2 = row(2);
    glm::vec4 const r3 = row(3);

    // Gribb-Hartmann frustum planes for Vulkan clip space (-w <= x <= w, -w <= y <= w, 0 <= z <= w).
    // Normals point inward: dot(plane.xyz, worldPos) + plane.w >= -radius means inside/intersecting.
    std::array<glm::vec4, 6U> planes{
        r3 + r0,// Left
        r3 - r0,// Right
        r3 + r1,// Top / Bottom (+Y clip)
        r3 - r1,// Bottom / Top (-Y clip)
        r2,     // Near (0 <= z_clip)
        r3 - r2,// Far  (z_clip <= w_clip)
    };

    for (auto& plane : planes)
    {
        float const normalLen = glm::length(glm::vec3{plane});
        if (normalLen > 0.0F)
        {
            plane /= normalLen;
        }
    }

    return planes;
}
}// namespace

auto GetCameraUniform(
    ecs::CameraComponent const*    tCamera,
    ecs::TransformComponent const* tTransform,
    std::uint32_t const            tScWidth,
    std::uint32_t const            tScHeight) -> CameraUniform
{
    // Compute forward vector from pitch and yaw
    glm::quat const pitch_quat = glm::angleAxis(glm::radians(tTransform->rotation.x), glm::vec3(1.0F, 0.0F, 0.0F));
    glm::quat const yaw_quat   = glm::angleAxis(glm::radians(tTransform->rotation.y), glm::vec3(0.0F, 1.0F, 0.0F));

    // Apply Yaw first, then Pitch
    glm::quat const orientation = yaw_quat * pitch_quat;
    glm::vec3 const forward     = orientation * glm::vec3{0.0F, 0.0F, -1.0F};
    glm::vec3 const up          = orientation * glm::vec3{0.0F, 1.0F, 0.0F};

    glm::mat4 const viewMatrix = glm::lookAt(tTransform->position, tTransform->position + forward, up);
    glm::mat4       projMatrix = glm::perspective(
        glm::radians(static_cast<std::float_t>(tCamera->fov)),
        static_cast<float>(tScWidth) / static_cast<float>(tScHeight),
        tCamera->nearZ,
        tCamera->farZ);

    projMatrix[1][1] *= -1;

    glm::mat4 const viewProj = projMatrix * viewMatrix;
    return {
        .viewProj      = viewProj,
        .frustumPlanes = ExtractFrustumPlanes(viewProj),
    };
}

auto CreateCameraUniformBuffer() -> std::unique_ptr<Buffer>
{
    auto buffer = Buffer::Create(
        BufferDescription{
            .size       = sizeof(CameraUniform),
            .usage      = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            .residence  = MemoryResidence::kHost,
            .hostAccess = HostAccessFlags::kSeqWrite,
        });
    PL_ASSERT(nullptr != buffer, "Camera uniform buffer could not be created");

    return buffer;
}

}// namespace polos::rendering
