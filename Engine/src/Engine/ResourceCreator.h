#pragma once

#include "Engine/Mesh.h"
#include "Engine/Program.h"
#include "Engine/Texture.h"

class ResourceCreator
{
    ResourceCreator() = delete;

public:
    static Mesh CreateTexturedQuad();

    static Program CreateProgram(const char* pVertexShaderFilename, const char* pFragmentShaderFilename);

    static Texture CreateTexture(const char* pTextureFilename);
};
