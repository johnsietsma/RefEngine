#pragma once

#include <map>

// A simple FBX material that supports 8 texture channels
class Material
{
public:
    enum class TextureType
    {
        DiffuseTexture = 0,
        AmbientTexture,
        GlowTexture,
        SpecularTexture,
        GlossTexture,
        NormalTexture,
        AlphaTexture,
        DisplacementTexture,
        Count
    };
    
    /*std:map<TextureType, const char*> textueNameMap {
        
    }*/

    static const char* getTextureName(TextureType textureIndex) {
        switch (textureIndex) {
        case TextureType::DiffuseTexture: return "diffuseSampler";
        case TextureType::AmbientTexture: return "ambientSampler";
        case TextureType::GlowTexture: return "glowSampler";
        case TextureType::SpecularTexture: return "specularSampler";
        case TextureType::GlossTexture: return "glossSampler";
        case TextureType::NormalTexture: return "normalSampler";
        case TextureType::AlphaTexture: return "alphaSampler";
        case TextureType::DisplacementTexture: return "displacementSampler";
        default: assert(false); return nullptr;
        }
    }

    Material();
    ~Material();
    
    bool isTextured() const;

    std::string     name;
    glm::vec4       ambient;                    // RGB + Ambient Factor stored in A
    glm::vec4       diffuse;                    // RGBA
    glm::vec4       specular;                   // RGB + Shininess/Gloss stored in A
    glm::vec4       emissive;                   // RGB + Emissive Factor stored in A

    std::string     texturePaths[(size_t)TextureType::Count];
    glm::vec2       textureOffsets[(size_t)TextureType::Count];         // Texture coordinate offset
    glm::vec2       textureTiling[(size_t)TextureType::Count];          // Texture repeat count
    float           textureRotation[(size_t)TextureType::Count];        // Texture rotation around Z (2D rotation)
};


inline Material::Material()
    : ambient(0, 0, 0, 0),
    diffuse(1, 1, 1, 1),
    specular(1, 1, 1, 1),
    emissive(0, 0, 0, 0),
    texturePaths{},
    textureOffsets{},
    textureTiling{},
    textureRotation{}
{
}

inline Material::~Material()
{
}


inline bool Material::isTextured() const
{
    for (size_t textureIndex = 0; textureIndex < (size_t)Material::TextureType::Count; textureIndex++) 
    {
        if( texturePaths[textureIndex].length() > 0 ) 
            return true;
    }
    return false;
}