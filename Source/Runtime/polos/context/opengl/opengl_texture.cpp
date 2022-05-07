#include "polos_pch.h"

#include <glad/glad.h>
#include <stb_image.h>

#include "context/texture.h"

namespace polos
{
    int32 Texture::s_IsFlipped = 0;

    Texture Texture::Load(cstring path)
    {
        if (!Texture::s_IsFlipped) stbi_set_flip_vertically_on_load(1);

        uint32 handle;
        glCreateTextures(GL_TEXTURE_2D, 1, &handle);

        glTextureParameteri(handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        int32 i_width;
        int32 i_height;
        int32 i_channels;
        auto* const pixel_data = stbi_load(path, &i_width, &i_height, &i_channels, 0);

        glTextureStorage2D(handle, 1, GL_RGBA8, i_width, i_height);
        glTextureSubImage2D(handle, 0, 0, 0, i_width, i_height, GL_RGBA, GL_UNSIGNED_BYTE, pixel_data);

        stbi_image_free(pixel_data);

        return {handle, i_width, i_height, i_channels};
    }

    Texture Texture::Load(std::string const& path)
    {
        return Load(path.c_str());
    }
}// namespace polos