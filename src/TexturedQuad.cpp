#include "TexturedQuad.h"

#include "gl_core_4_4.h"
#include "GeometryCreator.h"
#include "Helpers.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Vertex.h"

#include <assert.h>
#include <glm/vec2.hpp>
#include <string>

bool TexturedQuad::create(const glm::vec3& pos)
{
	m_transform.Translate(pos);

	std::string vertShader = ReadFile("./data/shaders/tex.vert");
	if (vertShader.length() == 0) return false;

	std::string fragShader = ReadFile("./data/shaders/tex.frag");
	if (fragShader.length() == 0) return false;

	if (!m_program.create(vertShader.c_str(), fragShader.c_str())) return false;


	// ---- Create the quad geo ----
	Vertex_PositionTexCoord* pVertices;
	unsigned int* pIndices;

	GeometryCreator::createTexuredQuad(&pVertices, &pIndices);

	m_mesh.create(pVertices, 6*4, pIndices, 6);

	delete[] pVertices;
	delete[] pIndices;


	// ---- Create the texture ----
	int imageWidth, imageHeight, imageComponents;
	unsigned char* data = stbi_load("./data/textures/sprite_sheet.png", &imageWidth, &imageHeight, &imageComponents, 3);
	if (data == nullptr) return false;

	assert(imageComponents == 3);
	assert(imageWidth > 0);
	assert(imageHeight > 0);

	m_texture.create(data, imageWidth, imageHeight);
	stbi_image_free(data);

	// Tell the sampler to look in texture unit 0
	m_program.setUniform("diffuseSampler", 0);

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