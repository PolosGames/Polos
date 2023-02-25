#pragma once

namespace polos
{
    // make this a REAL struct!!
    struct Texture
    {
        Texture();
        Texture(uint32 p_Id, int32 p_Width, int32 p_Height, int32 p_Channels);
        ~Texture();

        static std::shared_ptr<Texture> Load(std::string_view p_Path);

        static int32 s_IsFlipped;

        uint32 id{};
        int32  width{};
        int32  height{};
        int32  channels{};
    };

    using TextureRef = std::shared_ptr<Texture>;
}// namespace polos
