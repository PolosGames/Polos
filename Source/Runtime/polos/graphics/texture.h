#pragma once

namespace polos
{
    // make this a REAL struct!!
    struct Texture
    {
        inline static constexpr std::size_t k_MaxTextureNameSize = 256;

        Texture();
        Texture(uint32 p_Id, int32 p_Width, int32 p_Height, int32 p_Channels, std::string_view p_Name);
        ~Texture();
        
        static std::shared_ptr<Texture> Load(cstring p_ResourceName);

        uint32 id{};
        int32  width{};
        int32  height{};
        int32  channels{};
        CharArray<k_MaxTextureNameSize> textureName;
    };

    using TextureRef = std::shared_ptr<Texture>;
}// namespace polos
