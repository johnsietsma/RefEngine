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
    bool load(
        const char* a_filename,
        UNIT_SCALE a_scale = FBXFile::UNITS_METER,
        bool a_loadTextures = true,
        bool a_loadAnimations = true,
        bool a_loadMeshes = true,
        bool a_loadCameras = false,
        bool a_loadLights = false,
        bool a_flipTextureY = true
        );
        
    void unload();

    // the scene arranged in a tree graph
    FBXNode*  getRoot() const { return m_root; }

    // the ambient light of the scene
    const glm::vec4& getAmbientLight() const { return m_ambientLight; }

    const std::vector<FBXMeshNode*>& getMeshes() const { return m_meshes; }
    const std::map<std::string,FBXLightNode*>& getLights() const { return m_lights; }
    const std::map<std::string,FBXCameraNode*>& getCameras() const { return m_cameras; }
    const std::map<std::string,Material*>& getMaterials() const { return m_materials; }
    std::vector<FBXSkeleton*>& getSkeletons() { return m_skeletons; }
    const std::map<std::string,FBXAnimation*>& getAnimations() const { return m_animations; }

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

    Material*  extractMaterial(void* a_mesh, int a_materialIndex);

    static void optimiseMesh(FBXMeshNode* a_mesh);
    static void calculateTangentsBinormals(std::vector<Vertex_FBX>& a_vertices, const std::vector<unsigned int>& a_indices);

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

    ImportAssistor*                         m_importAssistor;
};

//////////////////////////////////////////////////////////////////////////



