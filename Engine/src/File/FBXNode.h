#pragma once

#include "Engine/Vertex.h"

#include <glm/mat4x4.hpp>

#include <string>
#include <vector>

class Material;


// Simple tree node with local/global transforms and children
// Also has a void* user data that the application can make use of
// for storing anything (i.e. the VAO/VBO/IBO data)
class FBXNode
{
public:

    enum class NodeType
    {
        NODE = 0,
        MESH,
        LIGHT,
        CAMERA
    };

    FBXNode() : FBXNode( NodeType::NODE ) {}
    
    FBXNode( NodeType nodeType ) :
        m_nodeType(nodeType ),
        m_localTransform(1), 
        m_globalTransform(1),
        m_parent(nullptr),
        m_userData(nullptr) 
    {}

    virtual ~FBXNode()
    {
        for (auto n : m_children)
            delete n;
    }

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



// A light node that can represent a point, directional, or spot light
class FBXLightNode : public FBXNode
{
public:

    FBXLightNode() : FBXNode(NodeType::LIGHT) {}
    virtual ~FBXLightNode() = default;

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

    FBXCameraNode() : FBXNode(NodeType::CAMERA) {}
    virtual ~FBXCameraNode() = default;

    // overridden to update m_viewMatrix automatically
    virtual void updateGlobalTransform() override;

    float       m_fieldOfView;  // if 0 then orthographic rather than perspective
    float       m_aspectRatio;  // if 0 then ratio based off screen resolution
    float       m_near;
    float       m_far;

    glm::mat4   m_viewMatrix;   // inverse matrix of node's m_globalTransform
};



