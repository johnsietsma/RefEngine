#pragma once

#include "Engine/Mesh.h"
#include "Engine/Program.h"
#include "Engine/Texture.h"

class ResourceCreator
{
    ResourceCreator() = delete;

public:
    static Program createProgram(const char* pVertexShaderFilename, const char* pFragmentShaderFilename);

    static Texture createTexture(const char* pTextureFilename);
};
