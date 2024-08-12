#pragma once

#include <glad/glad.h>

namespace polos
{
namespace graphics
{
    struct draw_data
    {
        GLenum      mode;
        GLint       first;
        GLsizei     count;
        GLenum      type;
        void const* indices;
        GLsizei     instance_count;
    };
}// namespace graphics
}// namespace polos
