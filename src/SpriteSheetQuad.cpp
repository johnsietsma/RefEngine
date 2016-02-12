#include "SpriteSheetQuad.h"

#include "gl_core_4_4.h"
#include "ResourceCreator.h"

#include <assert.h>

bool SpriteSheetQuad::create(const glm::vec3& pos, const char* pSpriteSheetFilename, int cellCountX, int cellCountY)
{
	m_cellCountX = cellCountX;
	m_cellCountY = cellCountY;

	m_transform.Translate(pos);


	m_program = ResourceCreator::CreateProgram("./data/shaders/tex.vert", "./data/shaders/spriteAnimation.frag");
	if (!m_program.isValid()) return false;

	// Tell the sampler to look in texture unit 0
	m_program.setUniform("diffuseSampler", 0);

	// ---- Create the quad geo ----
	m_mesh = ResourceCreator::CreateTexturedQuad();
	if (!m_mesh.isValid()) return false;

	// ---- Create the texture ----
	m_texture = ResourceCreator::CreateTexture(pSpriteSheetFilename);
	if (!m_texture.isValid()) return false;

	return true;
}


void SpriteSheetQuad::destroy()
{
	m_mesh.destroy();
	m_program.destroy();
	m_texture.destroy();
}


void SpriteSheetQuad::draw( const glm::mat4& projectionViewMatrix )
{
	// Use the program
	assert(m_program.isValid());
	glUseProgram(m_program.getId());
	m_program.setUniform("projectionView", projectionViewMatrix * m_transform.GetMatrix());

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