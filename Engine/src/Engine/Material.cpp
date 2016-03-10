#include "Material.h"

std::map<Material::TextureType, const char*> Material::TextueUnformNameMap {
    { Material::TextureType::DiffuseTexture, "diffuseSampler" },
    { Material::TextureType::AmbientTexture, "ambientSampler" },
    { Material::TextureType::GlowTexture, "glowSampler" },
    { Material::TextureType::SpecularTexture, "specularSampler" },
    { Material::TextureType::GlossTexture, "glossSampler" },
    { Material::TextureType::NormalTexture, "normalSampler" },
    { Material::TextureType::AlphaTexture, "alphaSampler" },
    { Material::TextureType::DisplacementTexture, "displacementSampler" }
};


bool Material::isTextured() const
{
    for (size_t textureIndex = 0; textureIndex < (size_t)Material::TextureType::Count; textureIndex++) 
    {
        if( texturePaths[textureIndex].length() > 0 ) 
            return true;
    }
    return false;
}