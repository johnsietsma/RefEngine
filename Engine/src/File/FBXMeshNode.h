#pragma once

#include "FBXNode.h"

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

    FBXMeshNode() : FBXNode( NodeType::MESH )
    {}

    void calculateTangentsAndBinormals();

    unsigned int                m_vertexAttributes;
    std::vector<Material*>      m_materials;
    std::vector<Vertex_FBX>     m_vertices;
    std::vector<unsigned int>   m_indices;
};
