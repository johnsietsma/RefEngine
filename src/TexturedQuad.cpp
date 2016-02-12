#include "TexturedQuad.h"

#include "gl_core_4_4.h"
#include "GeometryCreator.h"
#include "Helpers.h"
#include "ResourceCreator.h"

#include "Vertex.h"

#include <assert.h>
#include <glm/vec2.hpp>
#include <string>

bool TexturedQuad::create(const glm::vec3& pos)
{
	m_transform.Translate(pos);


	m_program = ResourceCreator::CreateProgram("./data/shaders/tex.vert", "./data/shaders/tex.frag");
	if (!m_program.isValid()) return false;

	// Tell the sampler to look in texture unit 0
	m_program.setUniform("diffuseSampler", 0);


	// ---- Create the quad geo ----
	m_mesh = ResourceCreator::CreateTexturedQuad();
	if (!m_mesh.isValid()) return false;

	// ---- Create the texture ----
	m_texture = ResourceCreator::CreateTexture("./data/textures/spritesheet.png");
	if (!m_texture.isValid()) return false;


	return true;
}


void TexturedQuad::destroy()
{
	m_mesh.destroy();
	m_program.destroy();
	m_texture.destroy();
}


void TexturedQuad::draw( const glm::mat4& projectionViewMatrix )
{
	// Use the program
	assert(m_program.isValid());
	glUseProgram(m_program.getId());
	m_program.setUniform("projectionView", projectionViewMatrix * m_transform.GetMatrix());

	// Bind the mesh
	assert(m_mesh.isValid());
	glBindVertexArray(m_mesh.getVAO());

	// Bind the texture to texture unit 0
	assert(m_texture.isValid());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture.getId());

	glDrawElements(GL_TRIANGLES, m_mesh.getIndexCount(), GL_UNSIGNED_INT, 0);

}