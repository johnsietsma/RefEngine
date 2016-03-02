#include "TexturedQuadGameObject.h"

#include "Engine/Camera.h"
#include "Engine/ResourceCreator.h"

#include "gl_core_4_4.h"

#include <assert.h>

bool TexturedQuadGameObject::create()
{
    Renderable renderable;

    renderable.program = ResourceCreator::CreateProgram("./data/shaders/textured.vert", "./data/shaders/textured.frag");
    if (!renderable.program.isValid()) return false;

    // Tell the sampler to look in texture unit 0
    renderable.program.setUniform("diffuseSampler", 0);

    //  Create the quad geo
    renderable.mesh = ResourceCreator::CreateTexturedQuad();
    if (!renderable.mesh.isValid()) return false;

    renderable.samplers.emplace_back(m_texture, 0);

    m_renderables.push_back(renderable);

    return true;
}


void TexturedQuadGameObject::destroy()
{

}
