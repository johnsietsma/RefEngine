#pragma once

#include "Engine/Texture.h"

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
struct Vertex_FBX;
class Material;

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

    FBXMeshNode();
    virtual ~FBXMeshNode();

    unsigned int                m_vertexAttributes;
    std::vector<Material*>      m_materials;
    std::vector<Vertex_FBX>      m_vertices;
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
    bool  load(
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
    Material*       getMaterialByName(const char* a_name);
    FBXAnimation*   getAnimationByName(const char* a_name);
    const Texture&  getTextureByName(const char* a_name) const;

    // these methods are slow as the items are stored in a map
    FBXMeshNode*    getMeshByIndex(unsigned int a_index) const { return m_meshes[a_index]; }
    FBXLightNode*   getLightByIndex(unsigned int a_index);
    FBXCameraNode*  getCameraByIndex(unsigned int a_index);
    Material*       getMaterialByIndex(unsigned int a_index);
    FBXSkeleton*    getSkeletonByIndex(unsigned int a_index) { return m_skeletons[a_index]; }
    FBXAnimation*   getAnimationByIndex(unsigned int a_index);
    const Texture&  getTextureByIndex(unsigned int a_index) const;

private:

    void    extractObject(FBXNode* a_parent, void* a_object);

    void    extractMeshes(void* pFbxMesh, FBXMeshNode& meshNode);
    void    extractLight(FBXLightNode* a_light, void* a_object);
    void    extractCamera(FBXCameraNode* a_camera, void* a_object);

    void    extractBonesAndAnimations(void* a_node, void* a_scene);
    void    gatherBones(void* a_object);
    void    extractSkeleton(FBXSkeleton* a_skeleton, void* a_scene);

    void    extractAnimation(void* a_scene);
    void    extractAnimationTrack(std::vector<int>& a_tracks, void* a_layer, void* a_node, std::vector<void*>& a_nodes, unsigned int& a_startFrame, unsigned int& a_endFrame);

    Material*    extractMaterial(void* a_mesh, int a_materialIndex);

    static void     optimiseMesh(FBXMeshNode* a_mesh);
    static void     calculateTangentsBinormals(std::vector<Vertex_FBX>& a_vertices, const std::vector<unsigned int>& a_indices);

    unsigned int    nodeCount(FBXNode* a_node);

private:

    FBXNode*                                m_root;

    std::string                             m_path;

    glm::vec4                               m_ambientLight;
    std::vector<FBXMeshNode*>               m_meshes;
    std::map<std::string, FBXLightNode*>    m_lights;
    std::map<std::string, FBXCameraNode*>   m_cameras;
    std::map<std::string, Material*>        m_materials;
    std::map<std::string, Texture>          m_textures;

    std::vector<FBXSkeleton*>               m_skeletons;
    std::map<std::string, FBXAnimation*>    m_animations;

    ImportAssistor*                         m_importAssistor;
};

//////////////////////////////////////////////////////////////////////////


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