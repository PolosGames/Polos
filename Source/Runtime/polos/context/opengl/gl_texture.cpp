#include "polos/polos_pch.h"

#include <glad/glad.h>
#include <stb_image.h>

#include "polos/context/texture.h"

namespace polos
{
    int32 Texture::s_IsFlipped = 0;

    Texture::Texture()
    {
    }

    Texture::Texture(uint32 id, int32 w, int32 h, int32 c)
        : id{id}, width{w}, height{h}, channels{c}
    {}

    Texture::~Texture()
    {
        glDeleteTextures(1, &id);
    }

    std::shared_ptr<Texture> Texture::Load(cstring path)
    {
        if (!Texture::s_IsFlipped) stbi_set_flip_vertically_on_load(1);

        uint32 handle;
        glCreateTextures(GL_TEXTURE_2D, 1, &handle);

        glTextureParameteri(handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        int32 i_width{};
        int32 i_height{};
        int32 i_channels{};
        
        if (path == nullptr)
        {
            glTextureStorage2D(handle, 1, GL_RGBA8, i_width, i_height);
            glTextureSubImage2D(handle, 0, 0, 0, i_width, i_height, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

            LOG_ENGINE_WARN("[Texture::Load] Creating a empty texture because the path provided was nullptr.");

            return std::make_shared<Texture>(handle, 0, 0, 4);
        }
        
        auto* const pixel_data = stbi_load(path, &i_width, &i_height, &i_channels, 4);

        if (pixel_data != nullptr)
        {
            auto const [sized_internal_format, internal_format] = [i_channels]() -> std::tuple<GLenum, GLenum> 
            {
                switch (i_channels)
                {
                    case 2: return {GL_RG8, GL_RG};
                    case 3: return {GL_RGB8, GL_RGB};
                    case 4: return {GL_RGBA8, GL_RGBA};
                    default: return {-1, -1};
                }
            }();

            glTextureStorage2D(handle, 1, sized_internal_format, i_width, i_height);
            glTextureSubImage2D(handle, 0, 0, 0, i_width, i_height, internal_format, GL_UNSIGNED_BYTE, pixel_data);

            stbi_image_free(pixel_data);

            return std::make_shared<Texture>(handle, i_width, i_height, i_channels);
        }

        LOG_ENGINE_ERROR("[Texture::Load] Failed to create a texture from the provided path");

        glDeleteTextures(1, &handle);

        return std::make_shared<Texture>(0, 0, 0, 0);
    }

    std::shared_ptr<Texture> Texture::Load(std::string const& path)
    {
        return Load(path.c_str());
    }
}// namespace polos