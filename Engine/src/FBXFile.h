#pragma once

#include <map>
#include <vector>
#include <string>
#include <thread>
#include <mutex>

#ifndef GLM_SWIZZLE
#define GLM_SWIZZLE
#endif // GLM_SWIZZLE

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/epsilon.hpp"

struct ImportAssistor;

// A complete vertex structure with all the data needed from the FBX file
class FBXVertex
{
public:

    enum VertexAttributeFlags
    {
        ePOSITION = (1 << 0),
        eCOLOUR = (1 << 1),
        eNORMAL = (1 << 2),
        eTANGENT = (1 << 3),
        eBINORMAL = (1 << 4),
        eINDICES = (1 << 5),
        eWEIGHTS = (1 << 6),
        eTEXCOORD1 = (1 << 7),
        eTEXCOORD2 = (1 << 8),
    };

    enum class Offsets
    {
        PositionOffset = 0,
        ColourOffset = PositionOffset + sizeof(glm::vec4),
        NormalOffset = ColourOffset + sizeof(glm::vec4),
        TangentOffset = NormalOffset + sizeof(glm::vec4),
        BiNormalOffset = TangentOffset + sizeof(glm::vec4),
        IndicesOffset = BiNormalOffset + sizeof(glm::vec4),
        WeightsOffset = IndicesOffset + sizeof(glm::vec4),
        TexCoord1Offset = WeightsOffset + sizeof(glm::vec4),
        TexCoord2Offset = TexCoord1Offset + sizeof(glm::vec2),
    };

    FBXVertex();
    ~FBXVertex();

    glm::vec4   position;
    glm::vec4   colour;
    glm::vec4   normal;
    glm::vec4   tangent;
    glm::vec4   binormal;
    glm::vec4   indices;
    glm::vec4   weights;
    glm::vec2   texCoord1;
    glm::vec2   texCoord2;

    bool operator == (const FBXVertex& a_rhs) const;
    bool operator < (const FBXVertex& a_rhs) const;

    // internal use only!
    unsigned int index[4];
};

enum class Offsets1
{
    PositionOffset = 0,
    ColourOffset = offsetof(class FBXVertex, colour),
    NormalOffset = offsetof(class FBXVertex, normal),
    TangentOffset = offsetof(class FBXVertex, tangent),
    BiNormalOffset = offsetof(class FBXVertex, binormal),
    IndicesOffset = offsetof(class FBXVertex, indices),
    WeightsOffset = offsetof(class FBXVertex, weights),
    TexCoord1Offset = offsetof(class FBXVertex, texCoord1),
    TexCoord2Offset = offsetof(class FBXVertex, texCoord2)
};


struct FBXTexture
{
    FBXTexture();
    ~FBXTexture();

    std::string     name;
    std::string     path;
    unsigned int    handle;
    unsigned char*  data;
    int             width;
    int             height;
    int             format;
};

// A simple FBX material that supports 8 texture channels
struct FBXMaterial
{
    enum TextureTypes
    {
        DiffuseTexture = 0,
        AmbientTexture,
        GlowTexture,
        SpecularTexture,
        GlossTexture,
        NormalTexture,
        AlphaTexture,
        DisplacementTexture,

        TextureTypes_Count
    };

    static const char* getTextureName(unsigned int textureIndex) {
        switch (textureIndex) {
        case DiffuseTexture: return "diffuseSampler";
        case AmbientTexture: return "ambientSampler";
        case GlowTexture: return "glowSampler";
        case SpecularTexture: return "specularSampler";
        case GlossTexture: return "glossSampler";
        case NormalTexture: return "normalSampler";
        case AlphaTexture: return "alphaSampler";
        case DisplacementTexture: return "displacementSampler";
        default: assert(false); return nullptr;
        }
    }

    FBXMaterial();
    ~FBXMaterial();

    std::string     name;
    glm::vec4       ambient;                    // RGB + Ambient Factor stored in A
    glm::vec4       diffuse;                    // RGBA
    glm::vec4       specular;                   // RGB + Shininess/Gloss stored in A
    glm::vec4       emissive;                   // RGB + Emissive Factor stored in A

    FBXTexture*     textures[TextureTypes_Count];
    glm::vec2       textureOffsets[TextureTypes_Count];         // Texture coordinate offset
    glm::vec2       textureTiling[TextureTypes_Count];          // Texture repeat count
    float           textureRotation[TextureTypes_Count];        // Texture rotation around Z (2D rotation)
};

// Simple tree node with local/global transforms and children
// Also has a void* user data that the application can make use of
// for storing anything (i.e. the VAO/VBO/IBO data)
class FBXNode
{
public:

    enum NodeType : unsigned int
    {
        NODE = 0,
        MESH,
        LIGHT,
        CAMERA
    };

    FBXNode();
    virtual ~FBXNode();

    // updates global transform based off parent's global and local
    // then updates children
    virtual void            updateGlobalTransform();

    NodeType                m_nodeType;
    std::string             m_name;

    glm::mat4               m_localTransform;
    glm::mat4               m_globalTransform;

    FBXNode*                m_parent;
    std::vector<FBXNode*>   m_children;

    void*                   m_userData;
};

// A simple mesh node that contains an array of vertices and indices used
// to represent a triangle mesh.
// Also points to a shared material, and stores a bitfield of vertex attributes
class FBXMeshNode : public FBXNode
{
public:

    FBXMeshNode();
    virtual ~FBXMeshNode();

    unsigned int                m_vertexAttributes;
    std::vector<FBXMaterial*>   m_materials;
    std::vector<FBXVertex>      m_vertices;
    std::vector<unsigned int>   m_indices;
};

// A light node that can represent a point, directional, or spot light
class FBXLightNode : public FBXNode
{
public:

    FBXLightNode();
    virtual ~FBXLightNode();

    enum LightType : unsigned int
    {
        Point = 0,
        Directional,
        Spot,
    };

    LightType   m_type;

    bool        m_on;
    glm::vec4   m_colour;       // RGB + Light Intensity stored in A
    glm::vec4   m_attenuation;  // (constant,linear,quadratic,0)
    float       m_innerAngle;   // spotlight inner cone angle (if a spotlight)
    float       m_outerAngle;   // spotlight outer cone angle (if a spotlight)
};

// A camera node with information to create projection matrix
class FBXCameraNode : public FBXNode
{
public:

    FBXCameraNode();
    virtual ~FBXCameraNode();

    // overridden to update m_viewMatrix automatically
    virtual void            updateGlobalTransform();

    float       m_fieldOfView;  // if 0 then orthographic rather than perspective
    float       m_aspectRatio;  // if 0 then ratio based off screen resolution
    float       m_near;
    float       m_far;

    glm::mat4   m_viewMatrix;   // inverse matrix of node's m_globalTransform
};

// A single frame for a bone in an animation
class FBXKeyFrame
{
public:

    FBXKeyFrame();
    ~FBXKeyFrame();

    unsigned int    m_key;
    glm::quat       m_rotation;
    glm::vec3       m_translation;
    glm::vec3       m_scale;
};

// A collection of frames for a single bone in an animation
class FBXTrack
{
public:

    FBXTrack();
    ~FBXTrack();

    unsigned int    m_boneIndex;
    unsigned int    m_keyframeCount;
    FBXKeyFrame*    m_keyframes;
};

// An animation that contains a collection of animated bone tracks
class FBXAnimation
{
public:

    FBXAnimation();
    ~FBXAnimation();

    // creates a deep-copy of this animation (caller takes ownership of returned data)
    FBXAnimation*   clone() const;

    unsigned int    totalFrames() const;
    float           totalTime(float a_fps = 24.0f) const;

    std::string     m_name;
    unsigned int    m_startFrame;
    unsigned int    m_endFrame;
    unsigned int    m_trackCount;
    FBXTrack*       m_tracks;
};

// A hierarchy of bones that can be animated
class FBXSkeleton
{
public:

    FBXSkeleton();
    ~FBXSkeleton();

    void            evaluate(const FBXAnimation* a_animation, float a_time, bool a_loop = true, float a_fps = 24.0f);
    void            updateBones();

    unsigned int    m_boneCount;
    FBXNode**       m_nodes;
    int*            m_parentIndex;

    glm::mat4*      m_bones;    // ready for use in skinning! (bind pose combined)
    glm::mat4*      m_bindPoses;

    void*           m_userData;
};

// An FBX scene representing the contents on an FBX file.
// Stores individual items within maps, with names as the key.
// Also has a pointer to the root of the scene's node tree.
class FBXFile
{
public:

    FBXFile() : m_root(nullptr), m_importAssistor(nullptr) {}
    ~FBXFile()
    {
        unload();
    }

    enum UNIT_SCALE
    {
        UNITS_MILLIMETER,
        UNITS_CENTIMETER,
        UNITS_DECIMETER,
        UNITS_METER,
        UNITS_KILOMETER,
        UNITS_INCH,
        UNITS_FOOT,
        UNITS_YARD,
        UNITS_MILE,
    };

    // must unload a scene before loading a new one over top
    bool            load(
        const char* a_filename,
        UNIT_SCALE a_scale = FBXFile::UNITS_METER,
        bool a_loadTextures = true,
        bool a_loadAnimations = true,
        bool a_loadMeshes = true,
        bool a_loadCameras = false,
        bool a_loadLights = false,
        bool a_flipTextureY = true
        );
    void            unload();

    // goes through all loaded textures and creates their GL versions
    void            initialiseOpenGLTextures();

    // the folder path of the FBX file
    // useful for accessing texture locations
    const char*         getPath() const { return m_path.c_str(); }

    // the scene arranged in a tree graph
    FBXNode*            getRoot() const { return m_root; }

    // the ambient light of the scene
    const glm::vec4&    getAmbientLight() const { return m_ambientLight; }

    size_t    getMeshCount() const { return m_meshes.size(); }
    size_t    getLightCount() const { return m_lights.size(); }
    size_t    getCameraCount() const { return m_cameras.size(); }
    size_t    getMaterialCount() const { return m_materials.size(); }
    size_t    getSkeletonCount() const { return m_skeletons.size(); }
    size_t    getAnimationCount() const { return m_animations.size(); }
    size_t    getTextureCount() const { return m_textures.size(); }

    FBXMeshNode*    getMeshByName(const char* a_name);
    FBXLightNode*   getLightByName(const char* a_name);
    FBXCameraNode*  getCameraByName(const char* a_name);
    FBXMaterial*    getMaterialByName(const char* a_name);
    FBXAnimation*   getAnimationByName(const char* a_name);
    FBXTexture*     getTextureByName(const char* a_name);

    // these methods are slow as the items are stored in a map
    FBXMeshNode*    getMeshByIndex(unsigned int a_index) const { return m_meshes[a_index]; }
    FBXLightNode*   getLightByIndex(unsigned int a_index);
    FBXCameraNode*  getCameraByIndex(unsigned int a_index);
    FBXMaterial*    getMaterialByIndex(unsigned int a_index);
    FBXSkeleton*    getSkeletonByIndex(unsigned int a_index) { return m_skeletons[a_index]; }
    FBXAnimation*   getAnimationByIndex(unsigned int a_index);
    FBXTexture*     getTextureByIndex(unsigned int a_index);

private:

    void    extractObject(FBXNode* a_parent, void* a_object);

    void    extractMeshes(void* pFbxMesh, FBXMeshNode& meshNode);
    void    extractLight(FBXLightNode* a_light, void* a_object);
    void    extractCamera(FBXCameraNode* a_camera, void* a_object);

    void    gatherBones(void* a_object);
    void    extractSkeleton(FBXSkeleton* a_skeleton, void* a_scene);

    void    extractAnimation(void* a_scene);
    void    extractAnimationTrack(std::vector<int>& a_tracks, void* a_layer, void* a_node, std::vector<void*>& a_nodes, unsigned int& a_startFrame, unsigned int& a_endFrame);

    FBXMaterial*    extractMaterial(void* a_mesh, int a_materialIndex);

    static void     optimiseMesh(FBXMeshNode* a_mesh);
    static void     calculateTangentsBinormals(std::vector<FBXVertex>& a_vertices, const std::vector<unsigned int>& a_indices);

    unsigned int    nodeCount(FBXNode* a_node);

private:

    FBXNode*                                m_root;

    std::string                             m_path;

    glm::vec4                               m_ambientLight;
    std::vector<FBXMeshNode*>               m_meshes;
    std::map<std::string, FBXLightNode*>    m_lights;
    std::map<std::string, FBXCameraNode*>   m_cameras;
    std::map<std::string, FBXMaterial*>     m_materials;
    std::map<std::string, FBXTexture*>      m_textures;

    std::vector<FBXSkeleton*>               m_skeletons;
    std::map<std::string, FBXAnimation*>    m_animations;

    ImportAssistor*                         m_importAssistor;
};

//////////////////////////////////////////////////////////////////////////
inline FBXVertex::FBXVertex()
    : position(0, 0, 0, 1),
    colour(1, 1, 1, 1),
    normal(0, 0, 0, 0),
    tangent(0, 0, 0, 0),
    binormal(0, 0, 0, 0),
    indices(0, 0, 0, 0),
    weights(0, 0, 0, 0),
    texCoord1(0, 0),
    texCoord2(0, 0)
{

}

inline FBXVertex::~FBXVertex()
{

}

inline bool FBXVertex::operator == (const FBXVertex& a_rhs) const
{
    return memcmp(this, &a_rhs, sizeof(FBXVertex)) == 0;
}

inline bool FBXVertex::operator < (const FBXVertex& a_rhs) const
{
    return memcmp(this, &a_rhs, sizeof(FBXVertex)) < 0;
}

inline FBXTexture::FBXTexture()
    : handle(0 - 1),
    data(nullptr),
    width(0),
    height(0),
    format(0)
{

}

inline FBXMaterial::FBXMaterial()
    : ambient(0, 0, 0, 0),
    diffuse(1, 1, 1, 1),
    specular(1, 1, 1, 1),
    emissive(0, 0, 0, 0)
{
    memset(textures, 0, TextureTypes_Count * sizeof(FBXTexture*));
    memset(textureOffsets, 0, TextureTypes_Count * sizeof(glm::vec2));
    memset(textureTiling, 0, TextureTypes_Count * sizeof(glm::vec2));
    memset(textureRotation, 0, TextureTypes_Count * sizeof(float));
}

inline FBXMaterial::~FBXMaterial()
{

}

inline FBXNode::FBXNode()
    : m_nodeType(NODE),
    m_localTransform(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1),
    m_globalTransform(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1),
    m_parent(nullptr),
    m_userData(nullptr)
{

}

inline FBXNode::~FBXNode()
{
    for (auto n : m_children)
        delete n;
}

inline FBXMeshNode::FBXMeshNode() :
    m_vertices(0),
    m_indices(0)
{
    m_nodeType = MESH;
}

inline FBXMeshNode::~FBXMeshNode()
{

}

inline FBXLightNode::FBXLightNode()
{
    m_nodeType = LIGHT;
}

inline FBXLightNode::~FBXLightNode()
{

}

inline FBXCameraNode::FBXCameraNode()
{
    m_nodeType = CAMERA;
}

inline FBXCameraNode::~FBXCameraNode()
{

}

inline FBXKeyFrame::FBXKeyFrame()
    : m_key(0),
    m_rotation(0, 0, 0, 1),
    m_translation(0, 0, 0),
    m_scale(1, 1, 1)
{

}

inline FBXKeyFrame::~FBXKeyFrame()
{

}

inline FBXTrack::FBXTrack()
    : m_boneIndex(0),
    m_keyframeCount(0),
    m_keyframes(nullptr)
{

}

inline FBXTrack::~FBXTrack()
{
    delete[] m_keyframes;
}

inline FBXAnimation::FBXAnimation()
    : m_startFrame(0xffffffff),
    m_endFrame(0),
    m_trackCount(0),
    m_tracks(nullptr)
{

}

inline FBXAnimation::~FBXAnimation()
{
    delete[] m_tracks;
}

inline unsigned int FBXAnimation::totalFrames() const
{
    return m_endFrame - m_startFrame;
}

inline float FBXAnimation::totalTime(float a_fps /* = 24.0f */) const
{
    return (m_endFrame - m_startFrame) / a_fps;
}

inline FBXSkeleton::FBXSkeleton()
    : m_boneCount(0),
    m_nodes(nullptr),
    m_parentIndex(nullptr),
    m_bones(nullptr),
    m_bindPoses(nullptr),
    m_userData(nullptr)
{

}

inline FBXSkeleton::~FBXSkeleton()
{
    delete[] m_parentIndex;
    delete[] m_nodes;
    free(m_bones);
    free(m_bindPoses);
}