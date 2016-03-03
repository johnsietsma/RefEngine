#pragma once


#include "gl_core_4_4.h"

#include "Program.h"
#include "Texture.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <bitset>
#include <memory>

class Camera;


struct RenderPassConfig
{
    glm::vec3 clearColor;
    glm::ivec2 fboSize;
    bool createColorBuffer;
    bool createDepthBuffer;
    Program fboProgram;
    std::bitset<8> layers;
};

class RenderPass
{
public:
    // The backbuffer
    RenderPass(std::weak_ptr<Camera> pCamera, glm::vec3 clearColor, glm::ivec2 size) :
        m_pCamera(pCamera),
        m_clearColor(clearColor),
        m_fboSize(size)
    {}

    RenderPass( std::weak_ptr<Camera> pCamera, const RenderPassConfig& config ) :
        m_fboId(0),
        m_fboTexture(-1),
        m_fboDepth(-1),
        m_pCamera(pCamera),
        m_clearColor(config.clearColor),
        m_fboSize(config.fboSize),
        m_createColorBuffer(config.createColorBuffer),
        m_createDepthBuffer(config.createDepthBuffer),
        m_fboProgram(config.fboProgram),
        m_layers(config.layers)
    {}

    bool create();
    void destroy();

    std::weak_ptr<Camera> getCamera() const { return m_pCamera; }
    GLuint getId() const { return m_fboId; }
    const Texture& getTexture() const { return m_fboTexture; }
    const GLuint getDepthBufferId() const { return m_fboDepth;  }
    const glm::vec3& getClearColor() const { return m_clearColor; }
    const glm::ivec2& getSize() const { return m_fboSize;  }
    const Program& getProgram() { return m_fboProgram;  }
    const std::bitset<8>& getLayers() const { return m_layers;  }

private:
    bool m_createColorBuffer;
    bool m_createDepthBuffer;
    GLuint m_fboId = 0;
    Texture m_fboTexture = -1;
    GLuint m_fboDepth = -1;
    std::weak_ptr<Camera> m_pCamera;
    glm::vec3 m_clearColor;
    glm::ivec2 m_fboSize;
    Program m_fboProgram; // Everything in this RenderPass will be rendered with the Program if it is set.
    std::bitset<8> m_layers = 0xFF;
};
