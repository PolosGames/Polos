#pragma once
#ifndef POLOS_RENDERER_CONTEXT_TEXTURE_H_
#define POLOS_RENDERER_CONTEXT_TEXTURE_H_

namespace polos
{
    class Texture
    {
    public:
        void LoadTexture();
    private:
        void* m_Data;
        int64 m_Handle;
    };
}// namespace polos

#endif /* POLOS_RENDERER_CONTEXT_TEXTURE_H_ */