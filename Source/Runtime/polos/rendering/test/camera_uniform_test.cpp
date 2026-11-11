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

TEST(CameraUniformTest, FrustumPlanesClassifySpheresAccurately)
{
    ecs::CameraComponent const    camera{.nearZ = 1.0F, .farZ = 100.0F, .fov = 90.0F};
    ecs::TransformComponent const transform{};

    auto const uniform = GetCameraUniform(&camera, &transform, 1000U, 1000U);

    auto const isSphereVisible = [&uniform](glm::vec3 const& tCenter, float const tRadius) -> bool {
        for (auto const& plane : uniform.frustumPlanes)
        {
            if (glm::dot(glm::vec3{plane}, tCenter) + plane.w < -tRadius)
            {
                return false;
            }
        }
        return true;
    };

    // Camera at origin looking down -Z with near=1, far=100, fov=90, aspect=1
    EXPECT_TRUE(isSphereVisible({0.0F, 0.0F, -10.0F}, 0.5F));   // Inside frustum
    EXPECT_FALSE(isSphereVisible({0.0F, 0.0F, 5.0F}, 0.5F));    // Behind camera
    EXPECT_FALSE(isSphereVisible({0.0F, 0.0F, -0.2F}, 0.5F));   // In front of near plane (z = -0.2, r = 0.5 < 0.8)
    EXPECT_TRUE(isSphereVisible({0.0F, 0.0F, -0.2F}, 0.9F));    // Intersecting near plane (r = 0.9 > 0.8)
    EXPECT_FALSE(isSphereVisible({0.0F, 0.0F, -105.0F}, 2.0F)); // Beyond far plane
    EXPECT_TRUE(isSphereVisible({0.0F, 0.0F, -105.0F}, 6.0F));  // Intersecting far plane
    EXPECT_FALSE(isSphereVisible({20.0F, 0.0F, -10.0F}, 1.0F)); // Outside right plane (at z=-10, x_max = 10)
    EXPECT_FALSE(isSphereVisible({-20.0F, 0.0F, -10.0F}, 1.0F));// Outside left plane
    EXPECT_FALSE(isSphereVisible({0.0F, 20.0F, -10.0F}, 1.0F)); // Outside top/bottom plane
    EXPECT_FALSE(isSphereVisible({0.0F, -20.0F, -10.0F}, 1.0F));// Outside bottom/top plane
}

}// namespace
}// namespace polos::rendering
