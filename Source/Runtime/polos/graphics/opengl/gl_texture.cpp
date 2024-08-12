#if defined(USE_OPENGL)

#include "polos/graphics/texture.h"

#include <glad/glad.h>
#include <stb_image.h>

#include "polos/core/resource/resource.h"
#include "polos/core/resource/types/image.h"

namespace polos
{
Texture::Texture() {}

Texture::Texture(uint32 p_Id, int32 p_Width, int32 p_Height, int32 p_Channels, std::string_view p_Name)
    : id{p_Id},
      width{p_Width},
      height{p_Height},
      channels{p_Channels},
      textureName(p_Name)
{}

Texture::~Texture()
{
    glDeleteTextures(1, &id);
}

std::shared_ptr<Texture> Texture::Load(cstring p_ResourceName)
{
    uint32 handle;
    glCreateTextures(GL_TEXTURE_2D, 1, &handle);

    glTextureParameteri(handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (p_ResourceName == nullptr)
    {
        LOG_ENGINE_ERROR("[Texture::Load] Resource name cannot be empty.");
        return std::make_shared<Texture>(handle, 0, 0, 4, "NoResource");
    }

    auto* img = resource::GetRawResource<resource::image>(p_ResourceName);

    if (img == nullptr)
    {
        LOG_ENGINE_ERROR("[Texture::Load] No resource could be found with name: {}", p_ResourceName);
        return std::make_shared<Texture>(handle, 0, 0, 4, "NoResource");
    }

    int32 i_width    = img->dimensions.x;
    int32 i_height   = img->dimensions.y;
    int32 i_channels = img->channels;

    auto* const pixel_data = img->data.data();

    if (pixel_data != nullptr)
    {
        auto const [sized_internal_format, internal_format] = [i_channels]() -> std::pair<GLenum, GLenum>
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

        //stbi_image_free(pixel_data);

        auto texture = std::make_shared<Texture>(handle, i_width, i_height, i_channels, p_ResourceName);

        return texture;
    }

    LOG_ENGINE_ERROR("[Texture::Load] Failed to create a texture from the provided path");

    glDeleteTextures(1, &handle);

    return std::make_shared<Texture>(0, 0, 0, 0, "PathIsNotATexture");
}

}// namespace polos

#endif// USE_OPENGL