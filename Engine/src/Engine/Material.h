#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <map>
#include <string>

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
    
    // Get a name that could be used for a uniform.
    static const char* getTextureUniformName(TextureType textureIndex) {
        return TextueUnformNameMap[textureIndex];
    }

    Material() = default;
    ~Material() = default;
    
    bool isTextured() const;

    std::string     name;
    glm::vec4       ambient = glm::vec4(0,0,0,0);                    // RGB + Ambient Factor stored in A
    glm::vec4       diffuse = glm::vec4(1,1,1,1);                    // RGBA
    glm::vec4       specular = glm::vec4(1,1,1,1);                   // RGB + Shininess/Gloss stored in A
    glm::vec4       emissive = glm::vec4(0,0,0,0);                   // RGB + Emissive Factor stored in A

    std::string     texturePaths[(size_t)TextureType::Count] {};
    glm::vec2       textureOffsets[(size_t)TextureType::Count] {};         // Texture coordinate offset
    glm::vec2       textureTiling[(size_t)TextureType::Count] {};          // Texture repeat count
    float           textureRotation[(size_t)TextureType::Count] {};        // Texture rotation around Z (2D rotation)
    
private:
    // Maps a TextureType to a a name that could be used by a uniform.
    static std::map<TextureType, const char*> TextueUnformNameMap;
};
