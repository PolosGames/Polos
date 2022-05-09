#pragma once
#ifndef POLOS_CONTEXT_TEXTURE_H_
#define POLOS_CONTEXT_TEXTURE_H_

namespace polos
{
    struct Texture
    {
        ~Texture();

        static Texture Load(cstring path);
        static Texture Load(std::string const& path);

        static int32 s_IsFlipped;

        uint32 id;
        int32  width;
        int32  height;
        int32  channels;
    };
}// namespace polos

#endif /* POLOS_CONTEXT_TEXTURE_H_ */