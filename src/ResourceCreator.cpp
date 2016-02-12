#include "ResourceCreator.h"

#include "GeometryCreator.h"
#include "Helpers.h"

#include "FBXFile.h"

#include "stb_image.h"

#include <string>


Mesh ResourceCreator::CreateTexturedQuad()
{
	Mesh mesh;

	Vertex_PositionTexCoord* pVertices;
	unsigned int* pIndices;

	GeometryCreator::createTexuredQuad(&pVertices, &pIndices);

	mesh.create(pVertices, 6 * 4, pIndices, 6);

	delete[] pVertices;
	delete[] pIndices;

	return mesh;
}


Program ResourceCreator::CreateProgram(const char* pVertexShaderFilename, const char* pFragmentShaderFilename)
{
	Program program;

	std::string vertShader = ReadFile(pVertexShaderFilename);
	if (vertShader.length() == 0) return program;

	std::string fragShader = ReadFile(pFragmentShaderFilename);
	if (fragShader.length() == 0) return program;

	if (!program.create(vertShader.c_str(), fragShader.c_str())) return program;

	return program;
}


Texture ResourceCreator::CreateTexture( const char* pTextureFilename )
{
	Texture texture;

	int imageWidth, imageHeight, imageComponents;
	unsigned char* data = stbi_load(pTextureFilename, &imageWidth, &imageHeight, &imageComponents, 3);
	if (data == nullptr) return texture;

	assert(imageComponents == 3);
	assert(imageWidth > 0);
	assert(imageHeight > 0);

	texture.create(data, imageWidth, imageHeight);

	stbi_image_free(data);

	return texture;
}

