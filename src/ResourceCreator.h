#pragma once

#include "Mesh.h"
#include "Program.h"
#include "Texture.h"

class ResourceCreator
{
	ResourceCreator() = delete;

public:
	static Mesh CreateTexturedQuad();

	static Program CreateProgram(const char* pVertexShaderFilename, const char* pFragmentShaderFilename);

	static Texture CreateTexture(const char* pTextureFilename);
};
