#pragma once

#include "FBXNode.h"

#include "Engine/Vertex.h"

#include <map>

#include <fbxsdk/fbxsdk_nsbegin.h>
class FbxLayerElementNormal;
class FbxLayerElementVertexColor;
class FbxLayerElementUV;
class FbxMesh;
class FbxVector4;
#include <fbxsdk/fbxsdk_nsend.h>

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
    
    void LoadVertexPositions( FBXSDK_NAMESPACE::FbxVector4* pVertexPositions, int vertexCount );
    void LoadVertexIndices( FBXSDK_NAMESPACE::FbxMesh* pFbxMesh);
    void LoadVertexColors( FBXSDK_NAMESPACE::FbxLayerElementVertexColor* pVertexColors, int vertexCount );
    void LoadTexCoords( FBXSDK_NAMESPACE::FbxLayerElementUV* pTexCoord, FBXSDK_NAMESPACE::FbxMesh* pFbxMesh, bool shouldFlipTextureY, int uvNumber);
    void LoadNormals( FBXSDK_NAMESPACE::FbxLayerElementNormal* pNormal, int vertexCount);
    void LoadSkinningData( FBXSDK_NAMESPACE::FbxMesh* pFbxMesh, std::map<std::string,int> boneIndexList );

    void calculateTangentsAndBinormals();

    unsigned int                m_vertexAttributes;
    std::vector<Material*>      m_materials;
    std::vector<Vertex_FBX>     m_vertices;
    std::vector<unsigned int>   m_indices;
};
