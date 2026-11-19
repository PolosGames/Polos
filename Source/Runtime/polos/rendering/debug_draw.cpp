///
/// Copyright (c) 2026 Kayra Urfali
/// Permission is hereby granted under the MIT License - see LICENSE for details.
///

#include "polos/rendering/debug_draw.hpp"

#include "polos/rendering/rendering_api.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace polos::rendering
{
namespace
{

void Enqueue(DebugPrimitive tPrimitive, glm::vec3 tPosition, glm::quat tRotation, glm::vec3 tScale, glm::vec4 tColor)
{
    RenderingApi::Get().GetDebugDrawQueue().Push(
        tPrimitive, DebugInstance{
                        .transform = glm::translate(glm::mat4{1.0F}, tPosition) * glm::mat4_cast(tRotation) *
                                     glm::scale(glm::mat4{1.0F}, tScale),
                        .color     = tColor,
                    });
}

}// namespace

void DebugDraw::Sphere(glm::vec3 tPosition, float tRadius, glm::vec4 tColor)
{
    Enqueue(DebugPrimitive::kSphere, tPosition, glm::quat{1.0F, 0.0F, 0.0F, 0.0F}, glm::vec3{tRadius}, tColor);
}

void DebugDraw::Circle(glm::vec3 tPosition, glm::quat tRotation, float tRadius, glm::vec4 tColor)
{
    Enqueue(DebugPrimitive::kCircle, tPosition, tRotation, glm::vec3{tRadius}, tColor);
}

void DebugDraw::Cube(glm::vec3 tPosition, glm::quat tRotation, glm::vec3 tSize, glm::vec4 tColor)
{
    Enqueue(DebugPrimitive::kCube, tPosition, tRotation, tSize, tColor);
}

}// namespace polos::rendering
