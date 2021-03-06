#include "ResourceCreator.h"

#include "graphics/Program.h"
#include "graphics/Texture.h"

#include "Helpers.h"

#define STB_IMAGE_IMPLEMENTATION
#pragma warning( disable : 4312 )
#include <stb_image.h>
#pragma warning( default : 4312 )

#include <iostream>
#include <string>


Program ResourceCreator::createProgram(const char* pVertexShaderName, const char* pFragmentShaderName)
{
    Program program;

    std::string vertexShaderFilename = std::string("./data/shaders/") + pVertexShaderName + ".vert";
    std::string fragmentShaderFilename = std::string("./data/shaders/") + pFragmentShaderName + ".frag";


    std::string vertShaderSource = ReadFile(vertexShaderFilename.c_str());
    if (vertShaderSource.length() == 0)
    {
        std::cerr << "Couldn't read file: " << vertexShaderFilename << std::endl;
        return program;
    }


    std::string fragShaderSource = ReadFile(fragmentShaderFilename.c_str());
    if (fragShaderSource.length() == 0) {
        std::cerr << "Couldn't read file: " << fragmentShaderFilename << std::endl;
        return program;
    }

    if (!program.create(vertShaderSource.c_str(), fragShaderSource.c_str()))
    {
        std::cerr << "Failed to make program with vertex shader " << vertexShaderFilename << " and fragment shader " << fragmentShaderFilename << "." << std::endl;
        return program;
    }

    return program;
}


Texture ResourceCreator::createTexture( const char* pTextureFilename )
{
    Texture texture;

    int imageWidth, imageHeight, imageComponents;
    unsigned char* data = stbi_load(pTextureFilename, &imageWidth, &imageHeight, &imageComponents, STBI_default );
    if (data == nullptr) return texture;

    assert(imageWidth > 0);
    assert(imageHeight > 0);

    texture.create(data, imageWidth, imageHeight, imageComponents);

    stbi_image_free(data);

    return texture;
}

