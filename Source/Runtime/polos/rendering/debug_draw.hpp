///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#ifndef POLOS_RENDERING_DEBUG_DRAW_HPP
#define POLOS_RENDERING_DEBUG_DRAW_HPP

#include "polos/polos_api.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <array>
#include <cstddef>
#include <vector>

namespace polos::rendering
{

enum class DebugPrimitive : std::size_t
{
    kSphere,
    kCircle,
    kCube,
    kCount,
};

inline constexpr std::size_t kDebugPrimitiveCount = static_cast<std::size_t>(DebugPrimitive::kCount);

// Matches DebugInstance in debug_draw.vert (std430).
struct alignas(16) DebugInstance
{
    glm::mat4 transform{1.0F};
    glm::vec4 color{1.0F};
};

static_assert(offsetof(DebugInstance, color) == 64U);
static_assert(sizeof(DebugInstance) == 80U);

struct DebugDrawQueue
{
    std::array<std::vector<DebugInstance>, kDebugPrimitiveCount> batches;

    void Push(DebugPrimitive tPrimitive, DebugInstance const& tInstance)
    {
        batches[static_cast<std::size_t>(tPrimitive)].push_back(tInstance);
    }

    void Clear()
    {
        for (auto& batch : batches)
        {
            batch.clear();
        }
    }
};

// Submit on the main thread before EndFrame. Shapes last for one frame.
class POLOS_API DebugDraw
{
public:
    static void Sphere(glm::vec3 tPosition, float tRadius, glm::vec4 tColor = glm::vec4{1.0F});

    // The unit circle lies in the XY plane, facing local +Z.
    static void Circle(glm::vec3 tPosition, glm::quat tRotation, float tRadius, glm::vec4 tColor = glm::vec4{1.0F});

    // Size is the full extent along each local axis.
    static void Cube(glm::vec3 tPosition, glm::quat tRotation, glm::vec3 tSize, glm::vec4 tColor = glm::vec4{1.0F});
};

}// namespace polos::rendering

#endif// POLOS_RENDERING_DEBUG_DRAW_HPP
