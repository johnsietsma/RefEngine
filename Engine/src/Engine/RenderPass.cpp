#include "RenderPass.h"

bool RenderPass::create()
{
    // setup framebuffer
    glGenFramebuffers(1, &m_fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);

    // Make the texture the FBO will render to.
    GLuint fboTextureId;
    glGenTextures(1, &fboTextureId);
    glBindTexture(GL_TEXTURE_2D, fboTextureId);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, m_fboSize.x, m_fboSize.y);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Attach the texture to the FBO
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, fboTextureId, 0);

    // Let the FBO know which attachment to render to
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    // Save the texture to be used by following render passes.
    m_fboTexture.setId(fboTextureId);

    // Create the depth buffer
    glGenRenderbuffers(1, &m_fboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, m_fboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_fboSize.x, m_fboSize.y);

    // Attach the depth buffer to the FBO
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_fboDepth);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

void RenderPass::destroy()
{
    GLuint texId = m_fboTexture.getId();
    glDeleteTextures(1, &texId);
    glDeleteRenderbuffers(1, &m_fboDepth);
}
