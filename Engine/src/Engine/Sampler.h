#pragma once

#include "Texture.h"

struct Sampler
{
    Sampler(Texture a_texture, unsigned int a_textureUnit) :
        texture(a_texture),
        textureUnit(a_textureUnit)
    {}

    Texture texture;
    unsigned int textureUnit;
};
