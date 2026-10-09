///
/// Copyright (c) 2025 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/uniforms/camera_uniform.hpp"

#include "polos/rendering/resources/buffer.hpp"
#include "polos/utils/assert.hpp"

namespace polos::rendering
{
auto GetCameraUniform(
    ecs::CameraComponent*    tCamera,
    ecs::TransformComponent* tTransform,
    std::uint32_t const      tScWidth,
    std::uint32_t const      tScHeight) -> CameraUniform
{
    // Compute forward vector from pitch and yaw
    glm::quat const pitch_quat = glm::angleAxis(glm::radians(tTransform->rotation.x), glm::vec3(1.0F, 0.0F, 0.0F));
    glm::quat const yaw_quat   = glm::angleAxis(glm::radians(tTransform->rotation.y), glm::vec3(0.0F, 1.0F, 0.0F));

    // Apply Yaw first, then Pitch
    glm::quat const orientation = yaw_quat * pitch_quat;
    glm::vec3 const forward     = orientation * glm::vec3{0.0F, 0.0F, -1.0F};
    glm::vec3 const up          = orientation * glm::vec3{0.0F, 1.0F, 0.0F};

    glm::mat4 const viewMatrix = glm::lookAt(tTransform->position, forward, up);
    glm::mat4       projMatrix = glm::perspective(
        glm::radians(static_cast<std::float_t>(tCamera->fov)),
        static_cast<float>(tScWidth) / static_cast<float>(tScHeight),
        tCamera->nearZ,
        tCamera->farZ);

    projMatrix[1][1] *= -1;

    return {.viewProj = viewMatrix * projMatrix};
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
