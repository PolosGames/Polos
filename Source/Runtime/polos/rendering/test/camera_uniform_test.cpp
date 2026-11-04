#include "polos/rendering/uniforms/camera_uniform.hpp"

#include <gtest/gtest.h>

#include <array>
#include <cmath>

namespace polos::rendering
{
namespace
{
TEST(CameraUniformTest, TranslatedCameraCentersThePointDirectlyAhead)
{
    ecs::CameraComponent    camera{.nearZ = 0.1F, .farZ = 50.0F, .fov = 55.0F};
    ecs::TransformComponent transform{.position = {4.0F, 3.0F, 8.0F}};
    glm::vec4 const         target{4.0F, 3.0F, 0.0F, 1.0F};

    auto const uniform = GetCameraUniform(&camera, &transform, 1280U, 720U);
    auto const clip    = uniform.viewProj * target;

    ASSERT_GT(clip.w, 0.0F);
    EXPECT_NEAR(clip.x / clip.w, 0.0F, 0.00001F);
    EXPECT_NEAR(clip.y / clip.w, 0.0F, 0.00001F);
    EXPECT_GT(clip.z / clip.w, 0.0F);
    EXPECT_LT(clip.z / clip.w, 1.0F);
}

TEST(CameraUniformTest, OverheadCameraFramesTheEntireTuxQuadGroup)
{
    struct Viewport
    {
        std::uint32_t width;
        std::uint32_t height;
        float         cameraHeight;
    };

    std::array const viewports{
        Viewport{1280U, 720U, 7.0F},
        Viewport{720U, 1280U, 18.0F},
    };

    ecs::CameraComponent    camera{.nearZ = 0.1F, .farZ = 50.0F, .fov = 55.0F};
    ecs::TransformComponent transform{
        .position = {0.0F, 7.0F, 0.0F},
        .rotation = {-90.0F, 0.0F, 0.0F},
    };
    std::array const corners{
        glm::vec4{-4.2F, 0.0F, -1.3F, 1.0F},
        glm::vec4{4.2F, 0.0F, -1.3F, 1.0F},
        glm::vec4{-4.2F, 0.0F, 1.3F, 1.0F},
        glm::vec4{4.2F, 0.0F, 1.3F, 1.0F},
    };

    for (auto const& viewport : viewports)
    {
        SCOPED_TRACE(::testing::Message() << "Viewport: " << viewport.width << 'x' << viewport.height);
        transform.position.y = viewport.cameraHeight;

        auto const uniform = GetCameraUniform(&camera, &transform, viewport.width, viewport.height);

        for (std::size_t i{0U}; i < corners.size(); ++i)
        {
            SCOPED_TRACE(::testing::Message() << "Corner: " << i);
            auto const clip = uniform.viewProj * corners[i];

            ASSERT_GT(clip.w, 0.0F);
            EXPECT_LT(std::abs(clip.x / clip.w), 1.0F);
            EXPECT_LT(std::abs(clip.y / clip.w), 1.0F);
            EXPECT_GT(clip.z / clip.w, 0.0F);
            EXPECT_LT(clip.z / clip.w, 1.0F);
        }
    }
}

}// namespace
}// namespace polos::rendering
