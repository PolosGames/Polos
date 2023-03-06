
#include <glad/glad.h>

#include "polos/graphics/framebuffer.h"

namespace polos
{
    FrameBuffer::FrameBuffer()
        :  m_DepthRbo{0},
        m_ColorAttachmentTexture{0}
    {
        m_ClearRGBA[0] = 0.45f;
        m_ClearRGBA[1] = 0.55f;
        m_ClearRGBA[2] = 0.6f;
        m_ClearRGBA[3] = 1.0f;

        m_ClearDepth[0] = 1.0f;

        glCreateFramebuffers(1, &m_Id);
    }

    FrameBuffer::~FrameBuffer()
    {
        if (m_DepthRbo != 0)
        {
            glDeleteTextures(1, &m_ColorAttachmentTexture);
        }
        if (m_ColorAttachmentTexture != 0)
        {
            glDeleteRenderbuffers(1, &m_DepthRbo);
        }
        glDeleteFramebuffers(1, &m_Id);
    }

    void FrameBuffer::Initialize(int32 width, int32 height)
    {
        // Create the texture for the color attachment
        glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachmentTexture);
        glTextureStorage2D(m_ColorAttachmentTexture, 1, GL_RGBA8, width, height);

        glTextureParameteri(m_ColorAttachmentTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(m_ColorAttachmentTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(m_ColorAttachmentTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_ColorAttachmentTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Create the renderbuffer object for the depth buffer. We make this as
        // a renderbuffer object because we don't want to change the depth
        // buffer in the shader in the near future. 
        // Also it is more efficient to store the depth buffer inside the rbo.
        // Since it doesn't create the overhead the texture does.
        glCreateRenderbuffers(1, &m_DepthRbo);
        glNamedRenderbufferStorage(m_DepthRbo, GL_DEPTH_COMPONENT, width, height);

        glNamedFramebufferRenderbuffer(m_Id, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthRbo);
        glNamedFramebufferTexture(m_Id, GL_COLOR_ATTACHMENT0, m_ColorAttachmentTexture, 0);

        if (glCheckNamedFramebufferStatus(m_Id, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            LOG_ENGINE_ERROR("Error while creating the framebuffer.");
    }

    uint32 FrameBuffer::GetFrameBufferHandle()
    {
        return m_Id;
    }

    uint32 FrameBuffer::GetFrameBufferTextureHandle()
    {
        return m_ColorAttachmentTexture;
    }

    void FrameBuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_Id);
        glNamedFramebufferDrawBuffer(m_Id, GL_COLOR_ATTACHMENT0);
    }

    void FrameBuffer::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBuffer::Clear()
    {
        glClearNamedFramebufferfv(m_Id, GL_COLOR, 0, m_ClearRGBA.data());
        glClearNamedFramebufferfv(m_Id, GL_DEPTH, 0, m_ClearDepth.data());
    }
} // namespace polos