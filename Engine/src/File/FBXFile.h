#pragma once

#include <glm/vec4.hpp>

#include <string>
#include <map>
#include <vector>

struct ImportAssistor;
struct Vertex_FBX;
class Material;

class FBXAnimation;
class FBXSkeleton;

class FBXNode;
class FBXCameraNode;
class FBXLightNode;
class FBXMeshNode;

enum class UnitScale
{
    MILLIMETER,
    CENTIMETER,
    DECIMETER,
    METER,
    KILOMETER,
    INCH,
    FOOT,
    YARD,
    MILE,
};

struct FBXLoadConfig
{
    bool loadTextures = true;
    bool loadAnimations = true;
    bool loadMeshes = true;
    bool loadCameras = false;
    bool loadLights = false;
    UnitScale unitScale = UnitScale::METER;
    bool flipTextureY = true;
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

    // must unload a scene before loading a new one over top
    bool load( const char* a_filename, FBXLoadConfig loadConfig=FBXLoadConfig() );
        
    void unload();

    // the scene arranged in a tree graph
    FBXNode*  getRoot() const { return m_root; }

    // the ambient light of the scene
    const glm::vec4& getAmbientLight() const { return m_ambientLight; }

    const std::vector<FBXMeshNode*>& getMeshes() const { return m_meshes; }
    const std::map<std::string,FBXLightNode*>& getLights() const { return m_lights; }
    const std::map<std::string,FBXCameraNode*>& getCameras() const { return m_cameras; }
    const std::map<std::string,Material*>& getMaterials() const { return m_materials; }
    std::vector<FBXSkeleton*>& getSkeletons() { return m_skeletons; } // Non-const to allow evaluation
    const std::map<std::string,FBXAnimation*>& getAnimations() const { return m_animations; }

private:
    void    extractObject(FBXNode* a_parent, void* a_object);

    void    extractMeshes(void* pFbxMesh, FBXMeshNode& meshNode);
    void    extractLight(FBXLightNode* a_light, void* a_object);
    void    extractCamera(FBXCameraNode* a_camera, void* a_object);

    void    extractBonesAndAnimations(void* a_scene);
    void    gatherBones(void* a_object);
    void    extractSkeleton(FBXSkeleton* a_skeleton, void* a_scene);

    void    extractAnimation(void* a_scene);
    void    extractAnimationTrack(std::vector<int>& a_tracks, void* a_layer, void* a_node, std::vector<void*>& a_nodes, unsigned int& a_startFrame, unsigned int& a_endFrame);

    Material*  extractMaterial(void* a_mesh, int a_materialIndex);

    unsigned int nodeCount(FBXNode* a_node);

private:

    FBXNode*                                m_root;

    std::string                             m_path;

    glm::vec4                               m_ambientLight;
    std::vector<FBXMeshNode*>               m_meshes;
    std::map<std::string, FBXLightNode*>    m_lights;
    std::map<std::string, FBXCameraNode*>   m_cameras;
    std::map<std::string, Material*>        m_materials;

    std::vector<FBXSkeleton*>               m_skeletons;
    std::map<std::string, FBXAnimation*>    m_animations;

    FBXLoadConfig                           m_loadConfig;
    ImportAssistor*                         m_importAssistor;
};

//////////////////////////////////////////////////////////////////////////



