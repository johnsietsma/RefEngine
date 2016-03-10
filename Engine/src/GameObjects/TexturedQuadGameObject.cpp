#include "TexturedQuadGameObject.h"

#include "engine/Camera.h"
#include "engine/GeometryCreator.h"
#include "engine/ResourceCreator.h"
#include "data/Vertex.h"

#include "gl_core_4_4.h"

#include <assert.h>

bool TexturedQuadGameObject::create()
{
    Renderable renderable;
    renderable.program = ResourceCreator::createProgram(m_vertShaderName.c_str(), m_fragShaderName.c_str());
    if (!renderable.program.isValid()) return false;

    // Tell the sampler to look in texture unit 0
    glUseProgram(renderable.program.getId());
    renderable.program.setUniform("diffuseSampler", 0);
    renderable.program.setUniform("shadowBias", 0.01f);

    //  Create the quad geo
    renderable.mesh = GeometryCreator::createTexturedQuad<Vertex_PositionNormalTexCoord>();
    if (!renderable.mesh.isValid())
        return false;

    renderable.samplers.emplace_back(m_texture, 0);

    m_renderables.push_back(renderable);

    return true;
}
