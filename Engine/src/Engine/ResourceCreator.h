#pragma once

class Program;
class Texture;

class ResourceCreator
{
    ResourceCreator() = delete;

public:
    static Program createProgram(const char* pVertexShaderFilename, const char* pFragmentShaderFilename);

    static Texture createTexture(const char* pTextureFilename);
};
