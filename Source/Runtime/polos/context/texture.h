#pragma once
#ifndef POLOS_RENDERER_CONTEXT_TEXTURE_H_
#define POLOS_RENDERER_CONTEXT_TEXTURE_H_

namespace polos
{
    struct Texture
    {
        uint32 id;
        
        int32 width;
        int32 height;
        int32 channels;

        static Texture Load(cstring path);
    };
}// namespace polos

#endif /* POLOS_RENDERER_CONTEXT_TEXTURE_H_ */