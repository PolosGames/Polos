#pragma once

namespace polos
{
    class FrameBuffer
    {
    public:
        FrameBuffer();
        ~FrameBuffer();

        void Initialize(int32 width, int32 height);

        uint32 GetFrameBufferHandle();
        uint32 GetFrameBufferTextureHandle();

        void Bind();
        void Unbind();

        void Clear();

    private:
        uint32 m_Id;
        uint32 m_DepthRbo;
        uint32 m_ColorAttachmentTexture;

        std::array<float, 4> m_ClearRGBA;
        std::array<float, 1> m_ClearDepth;
    };
} // namespace polos
