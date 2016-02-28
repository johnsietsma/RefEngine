#pragma once


#include "gl_core_4_4.h"

#include "Texture.h"

#include <glm/vec3.hpp>
#include <memory>

class Camera;

class RenderPass
{
public:
    RenderPass( std::weak_ptr<Camera> pCamera, glm::vec3 clearColor ) :
        m_fboId(0),
        m_fboTexture(-1),
        m_fboDepth(-1),
        m_pCamera(pCamera),
        m_clearColor(clearColor)
    {}

    bool create();
    void destroy();

    std::weak_ptr<Camera> getCamera() const { return m_pCamera; }
    GLuint getId() const { return m_fboId; }
    Texture getTexture() const { return m_fboTexture;  }
    glm::vec3 getClearColor() const { return m_clearColor; }

private:
    GLuint m_fboId;
    Texture m_fboTexture;
    GLuint m_fboDepth;
    std::weak_ptr<Camera> m_pCamera;
    glm::vec3 m_clearColor;
};
