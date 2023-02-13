#pragma once
#ifndef POLOS_CONTEXT_TEXTURE_H_
#define POLOS_CONTEXT_TEXTURE_H_

namespace polos
{
    // make this a REAL struct!!
    struct Texture
    {
        Texture();
        Texture(uint32 id, int32 w, int32 h, int32 c);
        ~Texture();

        static std::shared_ptr<Texture> Load(cstring path = nullptr);
        static std::shared_ptr<Texture> Load(std::string const& path);

        static int32 s_IsFlipped;

        uint32 id{};
        int32  width{};
        int32  height{};
        int32  channels{};
    };

    using TextureRef = std::shared_ptr<Texture>;
}// namespace polos

#endif /* POLOS_CONTEXT_TEXTURE_H_ */