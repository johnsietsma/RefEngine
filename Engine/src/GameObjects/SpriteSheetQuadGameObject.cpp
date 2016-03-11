#include "SpriteSheetQuadGameObject.h"

#include "engine/Camera.h"
#include "engine/GeometryCreator.h"
#include "engine/ResourceCreator.h"
#include "data/Vertex.h"

#include "gl_core_4_4.h"

#include <assert.h>

SpriteSheetQuadGameObject::SpriteSheetQuadGameObject(const Transform& trans, const char* pSpriteSheetFilename, int cellCountX, int cellCountY) :
    GameObject(trans),
    m_cellCountX(cellCountX),
    m_cellCountY(cellCountY),
    m_cellIndex(0),
    m_elapsedTime(0),
    m_filename(pSpriteSheetFilename)
{}


bool SpriteSheetQuadGameObject::create()
{
    // Create a program using a vertex shader that will pass through the texture coords and a frag shader that wil do the animation.
    auto program = ResourceCreator::createProgram("texturedFlipped", "spriteAnimation");
    if (!program.isValid()) return false;

    // Set up the sprite sheet uniforms
    glUseProgram(program.getId());
    program.setUniform("cellCountX", m_cellCountX);
    program.setUniform("cellCountY", m_cellCountY);

    // Tell the sampler in the fragment shader to look in texture unit 0
    glUseProgram(program.getId());
    program.setUniform("diffuseSampler", 0);

    auto mesh = GeometryCreator::createTexturedQuad<Vertex_PositionTexCoord>();
    if (!mesh.isValid()) return false;

    auto texture = ResourceCreator::createTexture(m_filename.c_str());
    if (!texture.isValid()) return false;

    Renderable rend(program, mesh, { Sampler(texture,0) });
    m_renderables.emplace_back(rend);

    return true;
}

void SpriteSheetQuadGameObject::update(float deltaTime)
{
    m_elapsedTime += deltaTime;

    const int timeMultiplier = 10;
    float strectchTime = m_elapsedTime * timeMultiplier;

    if (strectchTime - ((int)strectchTime) < deltaTime * timeMultiplier) {
        m_cellIndex++;
        m_cellIndex %= m_cellCountX * m_cellCountY;
    }
}


void SpriteSheetQuadGameObject::preDraw( const CameraComponent& camera, const Light& light )
{
    Program program = m_renderables[0].program;

	// Use the program
	assert(program.isValid());
	glUseProgram(program.getId());

	program.setUniform("cellIndex", m_cellIndex);
}
