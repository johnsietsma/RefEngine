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

    FBXMeshNode*    getMeshByName(const char* a_name);
    FBXLightNode*   getLightByName(const char* a_name);
    FBXCameraNode*  getCameraByName(const char* a_name);
    Material*       getMaterialByName(const char* a_name);
    FBXAnimation*   getAnimationByName(const char* a_name);

    // these methods are slow as the items are stored in a map
    FBXMeshNode*    getMeshByIndex(unsigned int a_index) const { return m_meshes[a_index]; }
    FBXLightNode*   getLightByIndex(unsigned int a_index);
    FBXCameraNode*  getCameraByIndex(unsigned int a_index);
    Material*       getMaterialByIndex(unsigned int a_index);
    FBXSkeleton*    getSkeletonByIndex(unsigned int a_index) { return m_skeletons[a_index]; }
    FBXAnimation*   getAnimationByIndex(unsigned int a_index);

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

    std::vector<FBXSkeleton*>               m_skeletons;
    std::map<std::string, FBXAnimation*>    m_animations;

    ImportAssistor*                         m_importAssistor;
};

//////////////////////////////////////////////////////////////////////////



