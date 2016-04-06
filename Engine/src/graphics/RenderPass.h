#pragma once


#include "RefOpenGL.h"

#include "Program.h"
#include "Texture.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <bitset>
#include <memory>

class CameraGameObject;


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
    RenderPass(std::weak_ptr<CameraGameObject> pCamera, glm::vec3 clearColor, glm::ivec2 size, GLuint fboId=0) :
        m_fboId(fboId),
        m_clearColor(clearColor),
        m_fboSize(size),
        m_pCamera(pCamera)
    {}
    

    RenderPass( std::weak_ptr<CameraGameObject> pCamera, const RenderPassConfig& config ) :
        m_fboId(0),
        m_clearColor(config.clearColor),
        m_fboSize(config.fboSize),
        m_fboTexture(-1),
        m_fboDepth(-1),
        m_createColorBuffer(config.createColorBuffer),
        m_createDepthBuffer(config.createDepthBuffer),
        m_pCamera(pCamera),
        m_fboProgram(config.fboProgram),
        m_layers(config.layers)
    {}

    bool create();
    void destroy();

    std::weak_ptr<CameraGameObject> getCamera() const { return m_pCamera; }
    GLuint getId() const { return m_fboId; }
    const Texture& getTexture() const { return m_fboTexture; }
    const GLuint getDepthBufferId() const { return m_fboDepth;  }
    const glm::vec3& getClearColor() const { return m_clearColor; }
    const glm::ivec2& getSize() const { return m_fboSize;  }
    const Program& getProgram() { return m_fboProgram;  }
    const std::bitset<8>& getLayers() const { return m_layers;  }

private:
    GLuint m_fboId = 0;
    glm::vec3 m_clearColor;
    glm::ivec2 m_fboSize;
    Texture m_fboTexture = -1;
    GLuint m_fboDepth = -1;
    bool m_createColorBuffer;
    bool m_createDepthBuffer;
    std::weak_ptr<CameraGameObject> m_pCamera;
    Program m_fboProgram; // Everything in this RenderPass will be rendered with the Program if it is set.
    std::bitset<8> m_layers = 0xFF;
};
