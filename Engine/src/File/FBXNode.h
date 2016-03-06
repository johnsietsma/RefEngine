#pragma once

#include "Engine/Vertex.h"

#include <glm/mat4x4.hpp>

#include <vector>

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
    std::vector<Vertex_FBX>     m_vertices;
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
