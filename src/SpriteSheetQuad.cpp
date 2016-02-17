#include "SpriteSheetQuad.h"

#include "Camera.h"
#include "gl_core_4_4.h"
#include "ResourceCreator.h"

#include <assert.h>

SpriteSheetQuad::SpriteSheetQuad(const glm::vec3& pos, const char* pSpriteSheetFilename, int cellCountX, int cellCountY) :
    GameObject(Transform(pos)),
    m_filename(pSpriteSheetFilename),
    m_cellCountX(cellCountX),
    m_cellCountY(cellCountY),
    m_cellIndex(0),
    m_elapsedTime(0)
{}


bool SpriteSheetQuad::create()
{
	m_program = ResourceCreator::CreateProgram("./data/shaders/tex.vert", "./data/shaders/spriteAnimation.frag");
	if (!m_program.isValid()) return false;

	// Tell the sampler to look in texture unit 0
	glUseProgram(m_program.getId());
	m_program.setUniform("diffuseSampler", 0);

	// ---- Create the quad geo ----
	m_mesh = ResourceCreator::CreateTexturedQuad();
	if (!m_mesh.isValid()) return false;

	// ---- Create the texture ----
	m_texture = ResourceCreator::CreateTexture(m_filename.c_str());
	if (!m_texture.isValid()) return false;

	return true;
}


void SpriteSheetQuad::destroy()
{
	m_mesh.destroy();
	m_program.destroy();
	m_texture.destroy();
}

void SpriteSheetQuad::update(float deltaTime)
{
    m_elapsedTime += deltaTime;


    const int timeMultiplier = 10;
    float strectchTime = m_elapsedTime * timeMultiplier;
    if (strectchTime - ((int)strectchTime) < deltaTime * timeMultiplier) {
        m_cellIndex++;
        m_cellIndex %= m_cellCountX * m_cellCountY;
    }
}


void SpriteSheetQuad::draw( const Camera& camera )
{
	// Use the program
	assert(m_program.isValid());
	glUseProgram(m_program.getId());
	m_program.setUniform("projectionView", camera.getProjectionView() * getTransform().GetMatrix());

	// Set up the sprite sheet uniforms
	m_program.setUniform("cellCountX", m_cellCountX);
	m_program.setUniform("cellCountY", m_cellCountY);
	m_program.setUniform("cellIndex", m_cellIndex);

	// Bind the mesh
	assert(m_mesh.isValid());
	glBindVertexArray(m_mesh.getVAO());

	// Bind the texture to texture unit 0
	assert(m_texture.isValid());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture.getId());

	glDrawElements(GL_TRIANGLES, m_mesh.getIndexCount(), GL_UNSIGNED_INT, 0);

}