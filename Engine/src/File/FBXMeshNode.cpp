#include "FBXMeshNode.h"

#include <fbxsdk.h>

#include <glm/vec3.hpp>
#include <glm/glm.hpp>


template<typename T>
static int GetDirectIndex( FbxLayerElementTemplate<T>* pElementArray, int defaultIndex )
{
    assert(pElementArray!=nullptr);
    
    int directIndex = defaultIndex;
    switch (pElementArray->GetReferenceMode())
    {
        case FbxGeometryElement::eDirect: break; // Just use the control point index in the direct array.
        case FbxGeometryElement::eIndexToDirect:
            directIndex = pElementArray->GetIndexArray().GetAt(defaultIndex);
            break;
        default:
            //assert_FAIL( "Reference mode not supported." );
            directIndex = -1;
            break;
    }
    return directIndex;
}

template<typename T>
static int GetPolygonVertexDirectIndex( FbxLayerElementTemplate<T>* pElementArray, FbxMesh* pFbxMesh, int polygonIndex, int vertIndex )
{
    int directIndex = -1;
    
    switch (pElementArray->GetReferenceMode())
    {
        case FbxGeometryElement::eDirect:
        case FbxGeometryElement::eIndexToDirect:
            directIndex = pFbxMesh->GetTextureUVIndex(polygonIndex, vertIndex);
            break;
        default:
            break; // other reference modes not shown here!
    }
    
    return directIndex;
}

static int GetVertexCoordDirectIndex( FbxMesh* pFbxMesh, FbxLayerElementUV* pFbxTexCoord, int controlPointIndex, int polygonIndex, int vertIndex)
{
    int directIndex = -1;
    switch (pFbxTexCoord->GetMappingMode())
    {
        case FbxGeometryElement::eByControlPoint:
            directIndex = GetDirectIndex(pFbxTexCoord, controlPointIndex);
            break;
            
        case FbxGeometryElement::eByPolygonVertex:
            directIndex = GetPolygonVertexDirectIndex(pFbxTexCoord, pFbxMesh, polygonIndex, vertIndex);
            break;
            
        case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
        case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
        case FbxGeometryElement::eNone:      // doesn't make much sense for UVs
        default: break;
    }
    
    return directIndex;
}

static int GetVertexColorDirectIndex( FbxGeometryElementVertexColor* pVertexColor, int controlPointIndex )
{
    int directIndex = -1;
    
    switch (pVertexColor->GetMappingMode())
    {
        case FbxGeometryElement::eByControlPoint:
            directIndex = GetDirectIndex(pVertexColor, controlPointIndex);
            break;
            
        case FbxGeometryElement::eByPolygonVertex:
        {
            switch (pVertexColor->GetReferenceMode())
            {
                case FbxGeometryElement::eDirect:
                case FbxGeometryElement::eIndexToDirect:
                    directIndex = GetDirectIndex(pVertexColor, controlPointIndex);
                    break;
                default:
                    break; // other reference modes not shown here!
            }
        }
            break;
            
        case FbxGeometryElement::eByPolygon:
        case FbxGeometryElement::eAllSame:
        case FbxGeometryElement::eNone:
        default: break;
    }
    
    return directIndex;
    
}



void FBXMeshNode::LoadVertexPositions( FbxVector4* pVertexPositions, int vertexCount )
{
    m_vertices.resize(vertexCount);
    
    for (int i = 0; i < vertexCount; ++i)
    {
        auto& vertex = m_vertices[i];
        FbxVector4 vPos = pVertexPositions[i];
        vertex.position.x = (float)vPos[0];
        vertex.position.y = (float)vPos[1];
        vertex.position.z = (float)vPos[2];
        vertex.position.w = 1;
    }
}

void FBXMeshNode::LoadVertexIndices( FbxMesh* pFbxMesh )
{
    int polygonCount = pFbxMesh->GetPolygonCount();
    m_indices.resize(polygonCount*3);
    
    unsigned int indexID = 0;
    for (int polygonIndex = 0; polygonIndex < polygonCount; polygonIndex++)
    {
        int polygonSize = pFbxMesh->GetPolygonSize(polygonIndex);
        for (int i = 0; i < polygonSize; i++)
        {
            assert(polygonSize==3);
            assert(indexID<m_indices.size());
            m_indices[indexID++] = pFbxMesh->GetPolygonVertex(polygonIndex, i);
        }
    }
    
}

void FBXMeshNode::LoadVertexColors( FbxLayerElementVertexColor* pVertexColors, int vertexCount )
{
    m_vertexAttributes |= FBXMeshNode::eCOLOUR;
    
    for (int i = 0; i < vertexCount; ++i)
    {
        auto& vertex = m_vertices[i];
        int directIndex = GetVertexColorDirectIndex(pVertexColors, i );
        if( directIndex >= 0 ) {
            FbxColor fbxColour = pVertexColors->GetDirectArray().GetAt(directIndex);
            
            vertex.colour.x = (float)fbxColour.mRed;
            vertex.colour.y = (float)fbxColour.mGreen;
            vertex.colour.z = (float)fbxColour.mBlue;
            vertex.colour.w = (float)fbxColour.mAlpha;
        }
    }
}

void FBXMeshNode::LoadTexCoords( FbxLayerElementUV* pTexCoord, FbxMesh* pFbxMesh, bool shouldFlipTextureY, int uvNumber)
{
    if( uvNumber == 0 ) {
        m_vertexAttributes |= FBXMeshNode::eTEXCOORD1;
    }
    else if( uvNumber == 1 ) {
        m_vertexAttributes |= FBXMeshNode::eTEXCOORD2;
    }
    else {
        assert( false );
        return;
    }

    int polygonCount = pFbxMesh->GetPolygonCount();
    
    for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
    {
        int polygonSize = pFbxMesh->GetPolygonSize(polygonIndex);
        
        for (int polyVertexIndex = 0; polyVertexIndex < polygonSize; ++polyVertexIndex)
        {
            assert(polyVertexIndex < 3);
            int vertexIndex = pFbxMesh->GetPolygonVertex(polygonIndex, polyVertexIndex);
            
            int directIndex = GetVertexCoordDirectIndex(pFbxMesh, pTexCoord, vertexIndex, polygonIndex, polyVertexIndex);
            if( vertexIndex!=-1 &&  directIndex>=0 ) {
                FbxVector2 fbxUV = pTexCoord->GetDirectArray().GetAt(directIndex);
                
                assert((unsigned int)vertexIndex < m_vertices.size());
                auto& vertex = m_vertices[vertexIndex];
                
                
                if( uvNumber == 0 ) {
                    vertex.texCoord1.x = (float)fbxUV[0];
                    vertex.texCoord1.y = (float)fbxUV[1];
                    
                    if (shouldFlipTextureY) vertex.texCoord1.y = 1.0f - vertex.texCoord1.y;
                }
                else if( uvNumber == 1 ) {
                    vertex.texCoord2.x = (float)fbxUV[0];
                    vertex.texCoord2.y = (float)fbxUV[1];
                    
                    if (shouldFlipTextureY) vertex.texCoord2.y = 1.0f - vertex.texCoord2.y;
                }
                
            }
        }
    }
    
}

void FBXMeshNode::LoadNormals(FbxLayerElementNormal* pNormal, int vertexCount )
{
    m_vertexAttributes |= FBXMeshNode::eNORMAL;

    for (int i = 0; i < vertexCount; ++i)
    {
        int directIndex = -1;
        
        if (pNormal->GetMappingMode() == FbxGeometryElement::eByControlPoint ||
            pNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
        {
            directIndex = GetDirectIndex(pNormal, i);
        }
        
        if( directIndex >= 0 )
        {
            FbxVector4 normal = pNormal->GetDirectArray().GetAt(directIndex);
            auto& vertex = m_vertices[i];
            vertex.normal.x = (float)normal[0];
            vertex.normal.y = (float)normal[1];
            vertex.normal.z = (float)normal[2];
            vertex.normal.w = 0;
        }
    }
}

void FBXMeshNode::LoadSkinningData( FbxMesh* pFbxMesh, std::map<std::string,int> boneIndexList )
{
    m_vertexAttributes |= FBXMeshNode::eINDICES | FBXMeshNode::eWEIGHTS;
    
    FbxSkin* fbxSkin = (FbxSkin*)pFbxMesh->GetDeformer(0, FbxDeformer::eSkin);
    int skinClusterCount = fbxSkin != nullptr ? fbxSkin->GetClusterCount() : 0;
    FbxCluster** skinClusters = nullptr;
    int* skinClusterBoneIndices = nullptr;
    if (skinClusterCount > 0)
    {
        skinClusters = new FbxCluster * [ skinClusterCount ];
        skinClusterBoneIndices = new int[ skinClusterCount ];
        
        for (int i = 0 ; i < skinClusterCount ; ++i)
        {
            skinClusters[i] = fbxSkin->GetCluster(i);
            if (skinClusters[i]->GetLink() == nullptr)
            {
                skinClusterBoneIndices[i] = -1;
            }
            else
            {
                skinClusterBoneIndices[i] = boneIndexList[ skinClusters[i]->GetLink()->GetName() ];
            }
        }
    }
    
    
    int polygonCount = pFbxMesh->GetPolygonCount();
    
    // process each polygon
    for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
    {
        int polygonSize = pFbxMesh->GetPolygonSize(polygonIndex);
        
        for (int polyVertexIndex = 0; polyVertexIndex < polygonSize && polyVertexIndex < 4 ; ++polyVertexIndex)
        {
            int vertexIndex = pFbxMesh->GetPolygonVertex(polygonIndex, polyVertexIndex);
            Vertex_FBX& vertex = m_vertices[vertexIndex];
            
            for (int skinClusterIndex = 0; skinClusterIndex != skinClusterCount; ++skinClusterIndex)
            {
                if (skinClusterBoneIndices[skinClusterIndex] == -1)
                    continue;
                
                int lIndexCount = skinClusters[skinClusterIndex]->GetControlPointIndicesCount();
                int* lIndices = skinClusters[skinClusterIndex]->GetControlPointIndices();
                double* lWeights = skinClusters[skinClusterIndex]->GetControlPointWeights();
                
                for (int l = 0; l < lIndexCount; l++)
                {
                    if (vertexIndex == lIndices[l])
                    {
                        // add weight and index
                        if (vertex.weights.x == 0)
                        {
                            vertex.weights.x = (float)lWeights[l];
                            vertex.indices.x = (float)skinClusterBoneIndices[skinClusterIndex];
                        }
                        else if (vertex.weights.y == 0)
                        {
                            vertex.weights.y = (float)lWeights[l];
                            vertex.indices.y = (float)skinClusterBoneIndices[skinClusterIndex];
                        }
                        else if (vertex.weights.z == 0)
                        {
                            vertex.weights.z = (float)lWeights[l];
                            vertex.indices.z = (float)skinClusterBoneIndices[skinClusterIndex];
                        }
                        else
                        {
                            vertex.weights.w = (float)lWeights[l];
                            vertex.indices.w = (float)skinClusterBoneIndices[skinClusterIndex];
                        }
                    }
                }
            }
        }
    }
    
    delete[] skinClusters;
    delete[] skinClusterBoneIndices;
}



void FBXMeshNode::calculateTangentsAndBinormals()
{
    if ((m_vertexAttributes & VertexAttributeFlags::eTEXCOORD1) == 0) return;
    
    m_vertexAttributes |= VertexAttributeFlags::eTANGENT|VertexAttributeFlags::eBINORMAL;
    
    size_t vertexCount = m_vertices.size();
    glm::vec3* tan1 = new glm::vec3[vertexCount * 2];
    glm::vec3* tan2 = tan1 + vertexCount;
    memset(tan1, 0, vertexCount * sizeof(glm::vec3) * 2);

    size_t indexCount = m_indices.size();
    for (unsigned int a = 0; a < indexCount; a += 3)
    {
        unsigned int i1 = m_indices[a];
        unsigned int i2 = m_indices[a + 1];
        unsigned int i3 = m_indices[a + 2];

        const glm::vec4& v1 = m_vertices[i1].position;
        const glm::vec4& v2 = m_vertices[i2].position;
        const glm::vec4& v3 = m_vertices[i3].position;

        const glm::vec2& w1 = m_vertices[i1].texCoord1;
        const glm::vec2& w2 = m_vertices[i2].texCoord1;
        const glm::vec2& w3 = m_vertices[i3].texCoord1;

        float x1 = v2.x - v1.x;
        float x2 = v3.x - v1.x;
        float y1 = v2.y - v1.y;
        float y2 = v3.y - v1.y;
        float z1 = v2.z - v1.z;
        float z2 = v3.z - v1.z;

        float s1 = w2.x - w1.x;
        float s2 = w3.x - w1.x;
        float t1 = w2.y - w1.y;
        float t2 = w3.y - w1.y;

        float t = s1 * t2 - s2 * t1;
        float r = t == 0 ? 0 : 1.0f / t;
        glm::vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
        glm::vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

        tan1[i1] += sdir;
        tan1[i2] += sdir;
        tan1[i3] += sdir;

        tan2[i1] += tdir;
        tan2[i2] += tdir;
        tan2[i3] += tdir;
    }

    for (size_t a = 0; a < vertexCount; a++)
    {
        const glm::vec3& n = m_vertices[a].normal.xyz();
        const glm::vec3& t = tan1[a];

        // Gram-Schmidt orthogonalise
        glm::vec3 p = t - n * glm::dot(n, t);
        if ( glm::length(p) != 0 )
        {
            m_vertices[a].tangent = glm::vec4( glm::normalize( p ), 0.0f );

            // calculate binormal
            float sign = glm::dot(glm::cross(n.xyz(), t.xyz()), tan2[a].xyz()) < 0.0f ? -1.0f : 1.0f;
            m_vertices[a].binormal = glm::vec4(glm::cross(m_vertices[a].normal.xyz(),m_vertices[a].tangent.xyz()) * sign, 0);
        }
    }

    delete[] tan1;    
}
