#include "FBXFile.h"

#include "Engine/Vertex.h"
#include "Engine/Material.h"

#include "gl_core_4_4.h"

#include <fbxsdk.h>


#define STB_IMAGE_IMPLEMENTATION
#pragma warning( disable : 4312 )
#include <stb_image.h>
#pragma warning( default : 4312 )

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>

#include <algorithm>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <set>

struct ImportAssistor
{
    ImportAssistor() :
        scene(nullptr),
        evaluator(nullptr),
        importer(nullptr),
        loadTextures(false),
        loadAnimations(false),
        loadMeshes(false),
        loadCameras(false),
        loadLights(false)
    {}
    
    ~ImportAssistor() = default;

    FbxScene*               scene;
    FbxAnimEvaluator*       evaluator;
    FbxImporter*            importer;
    std::vector<FBXNode*>   bones;

    bool                    loadTextures;
    bool                    loadAnimations;
    bool                    loadMeshes;
    bool                    loadCameras;
    bool                    loadLights;
    float                   unitScale;
    bool                    flipTextureY;

    std::map<std::string,int> boneIndexList;
};


//------------  Helpers --------------------

template<typename T>
int GetDirectIndex( FbxLayerElementTemplate<T>* pElementArray, int defaultIndex )
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
int GetPolygonVertexDirectIndex( FbxLayerElementTemplate<T>* pElementArray, FbxMesh* pFbxMesh, int polygonIndex, int vertIndex )
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

int GetVertexCoordDirectIndex( FbxMesh* pFbxMesh, FbxLayerElementUV* pFbxTexCoord, int controlPointIndex, int polygonIndex, int vertIndex)
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

int GetVertexColorDirectIndex( FbxGeometryElementVertexColor* pVertexColor, int controlPointIndex )
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

void LoadVertexPositions( FbxVector4* pVertexPositions, int vertexCount, std::vector<Vertex_FBX>& vertices )
{
    vertices.resize(vertexCount);
    
    for (int i = 0; i < vertexCount; ++i)
    {
        auto& vertex = vertices[i];
        FbxVector4 vPos = pVertexPositions[i];
        vertex.position.x = (float)vPos[0];
        vertex.position.y = (float)vPos[1];
        vertex.position.z = (float)vPos[2];
        vertex.position.w = 1;
    }
}

void LoadVertexIndices( FbxMesh* pFbxMesh, std::vector<unsigned int>& indices)
{
    int polygonCount = pFbxMesh->GetPolygonCount();
    indices.resize(polygonCount*3);
    
    unsigned int indexID = 0;
    for (int polygonIndex = 0; polygonIndex < polygonCount; polygonIndex++)
    {
        int polygonSize = pFbxMesh->GetPolygonSize(polygonIndex);
        for (int i = 0; i < polygonSize; i++)
        {
            assert(polygonSize==3);
            assert(indexID<indices.size());
            indices[indexID++] = pFbxMesh->GetPolygonVertex(polygonIndex, i);
        }
    }
    
}

void LoadVertexColors( FbxGeometryElementVertexColor* pVertexColors, int vertexCount, std::vector<Vertex_FBX>& vertices )
{
    for (int i = 0; i < vertexCount; ++i)
    {
        auto& vertex = vertices[i];
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

void LoadTexCoords( FbxLayerElementUV* pTexCoord, FbxMesh* pFbxMesh, bool shouldFlipTextureY, std::vector<Vertex_FBX>& vertices, int uvNumber)
{
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
                
                assert((unsigned int)vertexIndex < vertices.size());
                auto& vertex = vertices[vertexIndex];
                
                
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

void LoadNormals(FbxGeometryElementNormal* pNormal, int vertexCount, std::vector<Vertex_FBX>& vertices)
{
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
            auto& vertex = vertices[i];
            vertex.normal.x = (float)normal[0];
            vertex.normal.y = (float)normal[1];
            vertex.normal.z = (float)normal[2];
            vertex.normal.w = 0;
        }
    }
}

void LoadSkinningData( FbxMesh* pFbxMesh, std::vector<Vertex_FBX>& vertices, std::map<std::string,int> boneIndexList )
{
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
            Vertex_FBX& vertex = vertices[vertexIndex];
            
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


// ---------------- Class impl ----------------------


void FBXFile::unload()
{
    delete m_root;
    m_root = nullptr;

    for (auto m : m_meshes)
        delete m;
    for (auto m : m_materials)
        delete m.second;
    for (auto s : m_skeletons)
        delete s;
    for (auto a : m_animations)
        delete a.second;
    for (auto tex : m_textures)
        tex.second.destroy();


    m_meshes.clear();
    m_lights.clear();
    m_cameras.clear();
    m_materials.clear();
    m_skeletons.clear();
    m_animations.clear();
    m_textures.clear();
}

bool FBXFile::load(
                   const char* a_filename,
                   UNIT_SCALE a_scale /* = FBXFile::UNITS_METER */,
                   bool a_loadTextures /* = true */,
                   bool a_loadAnimations /* = true */,
                   bool a_loadMeshes /* = true */,
                   bool a_loadCameras /* = false */,
                   bool a_loadLights /* = false */,
                   bool a_flipTextureY /*= true*/
)
{
    if (m_root != nullptr)
    {
        printf("Scene already loaded!\n");
        return false;
    }

    std::ifstream ifs(a_filename, std::ios::in | std::ios::binary | std::ios::ate);
    if (!ifs.is_open()) {
        std::cerr << "File not found: " << a_filename << std::endl;
        return false;
    }

    FbxManager* lSdkManager = nullptr;
    FbxScene* lScene = nullptr;

    // The first thing to do is to create the FBX SDK manager which is the
    // object allocator for almost all the classes in the SDK.
    lSdkManager = FbxManager::Create();
    if ( lSdkManager==nullptr )
    {
        printf("Unable to create the FBX SDK manager\n");
        return false;
    }

    // create an IOSettings object
    FbxIOSettings * ios = FbxIOSettings::Create(lSdkManager, IOSROOT );
    lSdkManager->SetIOSettings(ios);

    // Create an importer.
    FbxImporter* lImporter = FbxImporter::Create(lSdkManager,"");

    // Initialize the importer by providing a filename.
    bool lImportStatus = lImporter->Initialize(a_filename, -1, lSdkManager->GetIOSettings());

    if ( !lImportStatus )
    {
        printf("Call to FbxImporter::Initialize() failed:\n\t%s\n", lImporter->GetStatus().GetErrorString());
        lImporter->Destroy();
        return false;
    }

    // Create the entity that will hold the scene.
    int lFileMajor, lFileMinor, lFileRevision;
    int lSDKMajor,  lSDKMinor,  lSDKRevision;

    unsigned int i;
    bool lStatus;

    // Get the file version number generate by the FBX SDK.
    FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);
    lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

    lScene = FbxScene::Create(lSdkManager,"root");

    // Import the scene.
    lStatus = lImporter->Import(lScene);
    lImporter->Destroy();
    if (lStatus == false)
    {
        printf("Unable to open FBX file!\n");
        return false;
    }

    float unitScale = 1;

    // convert scale
    if ( lScene->GetGlobalSettings().GetSystemUnit() != FbxSystemUnit::sPredefinedUnits[a_scale] )
    {
        const FbxSystemUnit::ConversionOptions lConversionOptions = {
            false, // mConvertRrsNodes
            true, // mConvertAllLimits
            true, // mConvertClusters
            true, // mConvertLightIntensity
            true, // mConvertPhotometricLProperties
            true  // mConvertCameraClipPlanes
        };

        unitScale = (float)(lScene->GetGlobalSettings().GetSystemUnit().GetScaleFactor() / FbxSystemUnit::sPredefinedUnits[a_scale].GetScaleFactor());

        // Convert the scene to meters using the defined options.
        FbxSystemUnit::sPredefinedUnits[a_scale].ConvertScene(lScene, lConversionOptions);
    }

    // convert the scene to OpenGL axis (right-handed Y up)
    FbxAxisSystem::OpenGL.ConvertScene(lScene);

    // Convert mesh, NURBS and patch into triangle mesh
    FbxGeometryConverter geomConverter(lSdkManager);
    geomConverter.Triangulate(lScene, true);

    FbxNode* lNode = lScene->GetRootNode();

    if (lNode != nullptr)
    {
        // store the folder path of the scene
        m_path = a_filename;
        size_t iLastForward = m_path.find_last_of('/');
        size_t iLastBackward = m_path.find_last_of('\\');
        if (iLastForward != std::string::npos)
        {
            m_path.resize(iLastForward + 1);
        }
        else if (iLastBackward != std::string::npos)
        {
            m_path.resize(iLastBackward + 1);
        }
        else
        {
            m_path = "";
        }

        m_importAssistor = new ImportAssistor();
        m_importAssistor->scene = lScene;
        m_importAssistor->evaluator = lScene->GetAnimationEvaluator();
        m_importAssistor->importer = lImporter;
        m_importAssistor->loadTextures = a_loadTextures;
        m_importAssistor->loadAnimations = a_loadAnimations;
        m_importAssistor->loadMeshes = a_loadMeshes;
        m_importAssistor->loadCameras = a_loadCameras;
        m_importAssistor->loadLights = a_loadLights;
        m_importAssistor->unitScale = unitScale;
        m_importAssistor->flipTextureY = a_flipTextureY;

        m_root = new FBXNode();
        m_root->m_name = "root";
        m_root->m_globalTransform = m_root->m_localTransform = glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);

        // grab the ambient light data from the scene
        FbxColor ambientColor = lScene->GetGlobalSettings().GetAmbientColor();
        m_ambientLight.x = (float)ambientColor.mRed;
        m_ambientLight.y = (float)ambientColor.mGreen;
        m_ambientLight.z = (float)ambientColor.mBlue;
        m_ambientLight.w = (float)ambientColor.mAlpha;

        // extract scene (meshes, lights, cameras)
        for (i = 0; i < (unsigned int)lNode->GetChildCount(); ++i)
        {
            extractObject(m_root, (void*)lNode->GetChild(i));
        }

        if (m_importAssistor->loadAnimations) {
            extractBonesAndAnimations(lNode, lScene);
        }

        m_root->updateGlobalTransform();

        delete m_importAssistor;
        m_importAssistor = nullptr;
    }

    lSdkManager->Destroy();

    return true;
}


void FBXFile::extractObject(FBXNode* a_parent, void* a_object)
{
    FbxNode* fbxNode = (FbxNode*)a_object;

    FBXNode* node = nullptr;

    FbxNodeAttribute::EType lAttributeType;
    int i;

    bool isBone = false;
    
    //JPS: Assuming only one attribute
    assert( fbxNode->GetNodeAttributeCount()<=1 );
    
    FbxNodeAttribute* pNodeAttribute = fbxNode->GetNodeAttribute();


    if (pNodeAttribute != nullptr)
    {
        lAttributeType = pNodeAttribute->GetAttributeType();

        switch (lAttributeType)
        {
        case FbxNodeAttribute::eSkeleton:
            {
                isBone = true;
            }
            break;

        case FbxNodeAttribute::eMesh:
            {
                if (m_importAssistor->loadMeshes)
                {
                    m_meshes.push_back(new FBXMeshNode());
                    FBXMeshNode& meshNode = *m_meshes.back();
                    meshNode.m_name = fbxNode->GetName();

                    extractMeshes(fbxNode->GetMesh(), meshNode);
                }
            }
            break;

        case FbxNodeAttribute::eCamera:
            {
                if (m_importAssistor->loadCameras == false)
                {
                    node = new FBXCameraNode();
                    extractCamera((FBXCameraNode*)node,fbxNode);

                    node->m_name = fbxNode->GetName();

                    m_cameras[node->m_name] = (FBXCameraNode*)node;
                }
            }
            break;

        case FbxNodeAttribute::eLight:
            {
                if (m_importAssistor->loadLights == false)
                {
                    node = new FBXLightNode();
                    extractLight((FBXLightNode*)node,fbxNode);

                    node->m_name = fbxNode->GetName();

                    m_lights[node->m_name] = (FBXLightNode*)node;
                }
            }
            break;

        default: break;
        }
    }


    // if null then use it as a plain 3D node
    if (node == nullptr)
    {
        node = new FBXNode();
        node->m_name = fbxNode->GetName();
    }

    // add to parent's children and update parent
    a_parent->m_children.push_back(node);
    node->m_parent = a_parent;

    // build local transform
    // use anim evaluator as bones store transforms in a different spot
    FbxAMatrix lLocal = m_importAssistor->evaluator->GetNodeLocalTransform(fbxNode);

    node->m_localTransform = glm::mat4( lLocal[0][0], lLocal[0][1], lLocal[0][2], lLocal[0][3],
                                        lLocal[1][0], lLocal[1][1], lLocal[1][2], lLocal[1][3],
                                        lLocal[2][0], lLocal[2][1], lLocal[2][2], lLocal[2][3],
                                        lLocal[3][0], lLocal[3][1], lLocal[3][2], lLocal[3][3] );

    if (m_importAssistor->loadAnimations == true &&
        isBone == true)
    {
        m_importAssistor->bones.push_back(node);
    }

    // children
    for (i = 0; i < fbxNode->GetChildCount(); i++)
    {
        extractObject(node, (void*)fbxNode->GetChild(i));
    }
}

void FBXFile::extractMeshes(void* a_object, FBXMeshNode& meshNode)
{
    assert(a_object!=nullptr);
    FbxMesh* pFbxMesh = static_cast<FbxMesh*>(a_object);
    

    FbxVector4* pVertexPositions = pFbxMesh->GetControlPoints();
    int vertexCount = pFbxMesh->GetControlPointsCount();
    
    LoadVertexPositions(pVertexPositions, vertexCount, meshNode.m_vertices);
    
    LoadVertexIndices(pFbxMesh, meshNode.m_indices);
    

    FbxGeometryElementVertexColor* fbxColours = pFbxMesh->GetElementVertexColor(0);
    if( fbxColours!=nullptr )
    {
        meshNode.m_vertexAttributes |= FBXMeshNode::eCOLOUR;
        LoadVertexColors( fbxColours, vertexCount, meshNode.m_vertices );
    }
    
    FbxGeometryElementUV* fbxTexCoord0 = pFbxMesh->GetElementUV(0);
    if( fbxTexCoord0 )
    {
        LoadTexCoords( fbxTexCoord0,  pFbxMesh, m_importAssistor->flipTextureY, meshNode.m_vertices, 0);
        meshNode.m_vertexAttributes |= FBXMeshNode::eTEXCOORD1;
    }

    FbxGeometryElementUV* fbxTexCoord1 = pFbxMesh->GetElementUV(1);
    if( fbxTexCoord1 )
    {
        LoadTexCoords( fbxTexCoord1,  pFbxMesh, m_importAssistor->flipTextureY, meshNode.m_vertices, 1);
        meshNode.m_vertexAttributes |= FBXMeshNode::eTEXCOORD2;
    }

    FbxGeometryElementNormal* fbxNormal = pFbxMesh->GetElementNormal(0);
    if( fbxNormal )
    {
        LoadNormals(fbxNormal, vertexCount, meshNode.m_vertices);
        meshNode.m_vertexAttributes |= FBXMeshNode::eNORMAL;

    }
    
    // gather skinning info
    LoadSkinningData(pFbxMesh, meshNode.m_vertices, m_importAssistor->boneIndexList);



    // set mesh names, vertex attributes, extract material and add to mesh map
    for ( int i = 0 ; i < pFbxMesh->GetElementMaterialCount(); ++i )
    {
        meshNode.m_materials.push_back( extractMaterial(pFbxMesh,i) );
    }

}

void FBXFile::optimiseMesh(FBXMeshNode* a_mesh)
{
    /*
    //sort the vertex array so all common verts are adjacent in the array
    std::sort(a_mesh->m_vertices.begin(), a_mesh->m_vertices.end());

    unsigned int forward_iter = 1;
    int j = 0;

    while ( forward_iter < a_mesh->m_vertices.size() )
    {
        if ( a_mesh->m_vertices[j] == a_mesh->m_vertices[forward_iter] )
        {
            // if the adjacent verts are equal make all the duplicate vert's indicies point at the first one in the vector
            a_mesh->m_indices[a_mesh->m_vertices[forward_iter].index[0]] = j;
            ++forward_iter;
        }
        else
        {
            // if they aren't duplicates, update the index to point at the vert's post sort position in the vector
            a_mesh->m_indices[a_mesh->m_vertices[j].index[0]] = j;
            ++j;
            // then push the current forward iterator back
            // not sure if checking if j != forward pointer would be faster here.
            a_mesh->m_vertices[j] = a_mesh->m_vertices[forward_iter];
            a_mesh->m_indices[a_mesh->m_vertices[forward_iter].index[0]] = j;
            ++forward_iter;
        }
    }
    a_mesh->m_vertices.resize(j+1);

    if ((a_mesh->m_vertexAttributes & Vertex_FBX::eTEXCOORD1) != 0)
    {
        a_mesh->m_vertexAttributes |= Vertex_FBX::eTANGENT|Vertex_FBX::eBINORMAL;
        calculateTangentsBinormals(a_mesh->m_vertices,a_mesh->m_indices);
    }
     */
}

void FBXFile::extractLight(FBXLightNode* a_light, void* a_object)
{
    FbxNode* fbxNode = (FbxNode*)a_object;
    FbxLight* fbxLight = (FbxLight*)fbxNode->GetNodeAttribute();

    // get type, if on, and colour
    a_light->m_type = (FBXLightNode::LightType)fbxLight->LightType.Get();
    a_light->m_on = fbxLight->CastLight.Get();
    a_light->m_colour.x = (float)fbxLight->Color.Get()[0];
    a_light->m_colour.y = (float)fbxLight->Color.Get()[1];
    a_light->m_colour.z = (float)fbxLight->Color.Get()[2];
    a_light->m_colour.w = (float)fbxLight->Intensity.Get();

    // get spot light angles (will return data even for non-spotlights)
    a_light->m_innerAngle = (float)fbxLight->InnerAngle.Get() * (glm::pi<float>() / 180);
    a_light->m_outerAngle = (float)fbxLight->OuterAngle.Get() * (glm::pi<float>() / 180);

    // get falloff data (none,linear, quadratic), cubic is ignored
    switch (fbxLight->DecayType.Get())
    {
    case 0:
        a_light->m_attenuation = glm::vec4(1,0,0,0);
        break;
    case 1:
        break;
        a_light->m_attenuation = glm::vec4(0,1,0,0);
    case 2:
        break;
        a_light->m_attenuation = glm::vec4(0,0,1,0);
    default:
        break;
    };
}

void FBXFile::extractCamera(FBXCameraNode* a_camera, void* a_object)
{
    FbxNode* fbxNode = (FbxNode*)a_object;
    FbxCamera* fbxCamera = (FbxCamera*)fbxNode->GetNodeAttribute();

    // get field of view
    if (fbxCamera->ProjectionType.Get() != FbxCamera::eOrthogonal)
    {
        a_camera->m_fieldOfView = (float)fbxCamera->FieldOfView.Get() * (glm::pi<float>() / 180);
    }
    else
    {
        a_camera->m_fieldOfView = 0;
    }

    // get aspect ratio if one was defined
    if (fbxCamera->GetAspectRatioMode() != FbxCamera::eWindowSize)
    {
        a_camera->m_aspectRatio = (float)fbxCamera->AspectWidth.Get() / (float)fbxCamera->AspectHeight.Get();
    }
    else
    {
        a_camera->m_aspectRatio = 0;
    }

    // get near/far
    a_camera->m_near = (float)fbxCamera->NearPlane.Get();
    a_camera->m_far = (float)fbxCamera->FarPlane.Get();

    // build view matrix
    glm::vec3 vEye, vTo, vUp;

    vEye.x = (float)fbxCamera->Position.Get()[0];
    vEye.y = (float)fbxCamera->Position.Get()[1];
    vEye.z = (float)fbxCamera->Position.Get()[2];

    if (fbxNode->GetTarget() != nullptr)
    {
        vTo.x = (float)fbxNode->GetTarget()->LclTranslation.Get()[0];
        vTo.y = (float)fbxNode->GetTarget()->LclTranslation.Get()[1];
        vTo.z = (float)fbxNode->GetTarget()->LclTranslation.Get()[2];
    }
    else
    {
        vTo.x = (float)fbxCamera->InterestPosition.Get()[0];
        vTo.y = (float)fbxCamera->InterestPosition.Get()[1];
        vTo.z = (float)fbxCamera->InterestPosition.Get()[2];
    }

    if (fbxNode->GetTargetUp())
    {
        vUp.x = (float)fbxNode->GetTargetUp()->LclTranslation.Get()[0];
        vUp.y = (float)fbxNode->GetTargetUp()->LclTranslation.Get()[1];
        vUp.z = (float)fbxNode->GetTargetUp()->LclTranslation.Get()[2];

    }
    else
    {
        vUp.x = (float)fbxCamera->UpVector.Get()[0];
        vUp.y = (float)fbxCamera->UpVector.Get()[1];
        vUp.z = (float)fbxCamera->UpVector.Get()[2];
    }

    a_camera->m_viewMatrix = glm::lookAt(vEye,vTo,vUp);
}

Material* FBXFile::extractMaterial(void* a_mesh, int a_materialIndex)
{
    FbxGeometry* pGeometry = (FbxGeometry*)a_mesh;
    FbxNode* lNode = pGeometry->GetNode();
    FbxSurfaceMaterial *lMaterial = lNode->GetMaterial(a_materialIndex);

    // check if material already loaded, else create new material
    auto oIter = m_materials.find( lMaterial->GetName() );
    if (oIter != m_materials.end())
    {
        return oIter->second;
    }
    else
    {
        Material* material = new Material;
        material->name = lMaterial->GetName();

        // get the implementation to see if it's a hardware shader.
        const FbxImplementation* lImplementation = GetImplementation(lMaterial, FBXSDK_IMPLEMENTATION_HLSL);
        if (lImplementation == nullptr)
        {
            lImplementation = GetImplementation(lMaterial, FBXSDK_IMPLEMENTATION_CGFX);
        }
        if (lImplementation != nullptr)
        {
            FbxBindingTable const* lRootTable = lImplementation->GetRootTable();
            FbxString lFileName = lRootTable->DescAbsoluteURL.Get();
            FbxString lTechniqueName = lRootTable->DescTAG.Get();

            printf("Unsupported hardware shader material!\n");
            printf("\tFile: %s\n",lFileName.Buffer());
            printf("\tTechnique: %s\n\n",lTechniqueName.Buffer());
        }
        else if (lMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
        {
            // We found a Phong material
            FbxSurfacePhong* pPhong = (FbxSurfacePhong*)lMaterial;

            material->ambient.x = (float)pPhong->Ambient.Get()[0];
            material->ambient.y = (float)pPhong->Ambient.Get()[1];
            material->ambient.z = (float)pPhong->Ambient.Get()[2];
            material->ambient.w = (float)pPhong->AmbientFactor.Get();

            material->diffuse.x = (float)pPhong->Diffuse.Get()[0];
            material->diffuse.y = (float)pPhong->Diffuse.Get()[1];
            material->diffuse.z = (float)pPhong->Diffuse.Get()[2];
            material->diffuse.w = (float)pPhong->TransparencyFactor.Get();

            material->specular.x = (float)pPhong->Specular.Get()[0];
            material->specular.y = (float)pPhong->Specular.Get()[1];
            material->specular.z = (float)pPhong->Specular.Get()[2];
            material->specular.w = (float)pPhong->Shininess.Get();

            material->emissive.x = (float)pPhong->Emissive.Get()[0];
            material->emissive.y = (float)pPhong->Emissive.Get()[1];
            material->emissive.z = (float)pPhong->Emissive.Get()[2];
            material->emissive.w = (float)pPhong->EmissiveFactor.Get();
        }
        else if(lMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId) )
        {
            FbxSurfaceLambert* pLambert = (FbxSurfaceLambert*)lMaterial;

            material->ambient.x = (float)pLambert->Ambient.Get()[0];
            material->ambient.y = (float)pLambert->Ambient.Get()[1];
            material->ambient.z = (float)pLambert->Ambient.Get()[2];
            material->ambient.w = (float)pLambert->AmbientFactor.Get();

            material->diffuse.x = (float)pLambert->Diffuse.Get()[0];
            material->diffuse.y = (float)pLambert->Diffuse.Get()[1];
            material->diffuse.z = (float)pLambert->Diffuse.Get()[2];
            material->diffuse.w = (float)pLambert->TransparencyFactor.Get();

            // No specular in lambert materials
            material->specular.x = 0;
            material->specular.y = 0;
            material->specular.z = 0;
            material->specular.w = 0;

            material->emissive.x = (float)pLambert->Emissive.Get()[0];
            material->emissive.y = (float)pLambert->Emissive.Get()[1];
            material->emissive.z = (float)pLambert->Emissive.Get()[2];
            material->emissive.w = (float)pLambert->EmissiveFactor.Get();
        }
        else
        {
            printf("Unknown type of Material: %s\n", lMaterial->GetClassId().GetName());
        }

        unsigned int auiTextureLookup[] =
        {
            FbxLayerElement::eTextureDiffuse - FbxLayerElement::sTypeTextureStartIndex,
            FbxLayerElement::eTextureAmbient - FbxLayerElement::sTypeTextureStartIndex,
            FbxLayerElement::eTextureEmissive - FbxLayerElement::sTypeTextureStartIndex,
            FbxLayerElement::eTextureSpecular - FbxLayerElement::sTypeTextureStartIndex,
            FbxLayerElement::eTextureShininess - FbxLayerElement::sTypeTextureStartIndex,
            FbxLayerElement::eTextureNormalMap - FbxLayerElement::sTypeTextureStartIndex,
            FbxLayerElement::eTextureTransparency - FbxLayerElement::sTypeTextureStartIndex,
            FbxLayerElement::eTextureDisplacement - FbxLayerElement::sTypeTextureStartIndex,
        };

        if (m_importAssistor->loadTextures == true)
        {
            for ( unsigned int i = 0 ; i < (size_t)Material::TextureType::Count ; ++i )
            {
                FbxProperty pProperty = lMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[auiTextureLookup[i]]);
                if ( pProperty.IsValid() &&
                    pProperty.GetSrcObjectCount<FbxTexture>() > 0)
                {
                    FbxFileTexture* fileTexture = FbxCast<FbxFileTexture>(pProperty.GetSrcObject<FbxTexture>(0));
                    if (fileTexture != nullptr)
                    {
                        const char* szLastForward = strrchr(fileTexture->GetFileName(),'/');
                        const char* szLastBackward = strrchr(fileTexture->GetFileName(),'\\');
                        const char* szFilename = fileTexture->GetFileName();

                        material->textureRotation[i] = (float)fileTexture->GetRotationW();
                        material->textureTiling[i].x = (float)fileTexture->GetScaleU();
                        material->textureTiling[i].y = (float)fileTexture->GetScaleV();
                        material->textureOffsets[i].x = (float)fileTexture->GetTranslationU();
                        material->textureOffsets[i].y = (float)fileTexture->GetTranslationV();

                        if (szLastForward != nullptr && szLastForward > szLastBackward)
                            szFilename = szLastForward + 1;
                        else if (szLastBackward != nullptr)
                            szFilename = szLastBackward + 1;

                        std::string fullPath = m_path + szFilename;

                        material->texturePaths[i] = fullPath;
                    }
                }
            }
        }

        m_materials[material->name] = material;
        return material;
    }

    return nullptr;
}

void FBXFile::extractBonesAndAnimations(void* a_node, void* a_scene)
{
    FbxNode* lNode = (FbxNode*)a_node;
    // gather bones to create indices for them in a skeleton
    for (unsigned int i = 0; i < (unsigned int)lNode->GetChildCount(); ++i)
    {
        gatherBones((void*)lNode->GetChild(i));
    }


    // build skeleton and extract animation keyframes
    if (m_importAssistor->bones.size() > 0)
    {
        FBXSkeleton* skeleton = new FBXSkeleton();
        skeleton->m_boneCount = (unsigned int)m_importAssistor->bones.size();

        skeleton->m_nodes = new FBXNode *[skeleton->m_boneCount];

        void* pBonesBuffer = malloc(sizeof(glm::mat4)*(skeleton->m_boneCount + 1));
        skeleton->m_bones = new(pBonesBuffer) glm::mat4[skeleton->m_boneCount];

        void* pBindPosesBuffer = malloc(sizeof(glm::mat4)*(skeleton->m_boneCount + 1));
        skeleton->m_bindPoses = new(pBindPosesBuffer)glm::mat4[skeleton->m_boneCount];

        skeleton->m_parentIndex = new int[skeleton->m_boneCount];

        for (unsigned int i = 0; i < skeleton->m_boneCount; ++i)
        {
            skeleton->m_nodes[i] = m_importAssistor->bones[i];
            skeleton->m_bones[i] = skeleton->m_nodes[i]->m_localTransform;
        }
        for (unsigned int i = 0; i < skeleton->m_boneCount; ++i)
        {
            skeleton->m_parentIndex[i] = -1;
            for (int j = 0; j < (int)skeleton->m_boneCount; ++j)
            {
                if (skeleton->m_nodes[i]->m_parent == skeleton->m_nodes[j])
                {
                    skeleton->m_parentIndex[i] = j;
                    break;
                }
            }
        }

        extractSkeleton(skeleton, a_scene);

        m_skeletons.push_back(skeleton);

        extractAnimation(a_scene);
    }

}

void FBXFile::extractAnimation(void* a_scene)
{
    FbxScene* fbxScene = (FbxScene*)a_scene;

    int animStackCount = fbxScene->GetSrcObjectCount<FbxAnimStack>();

    std::vector<int> tracks;
    std::vector<void*> nodes;
    tracks.reserve(128);
    nodes.reserve(128);

    FbxTime frameTime;

    for (int i = 0; i < animStackCount; i++)
    {
        tracks.clear();
        nodes.clear();

        FbxAnimStack* lAnimStack = fbxScene->GetSrcObject<FbxAnimStack>(i);

        FBXAnimation* anim = new FBXAnimation();
        anim->m_name = lAnimStack->GetName();

        // get animated track bone indices and nodes, and calculate start/end frame
        int nbAnimLayers = lAnimStack->GetMemberCount(FbxCriteria::ObjectType(FbxAnimLayer::ClassId));
        for (int j = 0; j < nbAnimLayers; ++j)
        {
            FbxAnimLayer* lAnimLayer = lAnimStack->GetMember<FbxAnimLayer>(j);
            extractAnimationTrack(tracks, lAnimLayer, fbxScene->GetRootNode(), nodes, anim->m_startFrame, anim->m_endFrame);
        }

        // frame count (includes start/end frames)
        unsigned int frameCount = anim->m_endFrame - anim->m_startFrame + 1;

        // allocate tracks and keyframes
        anim->m_trackCount = (unsigned int)tracks.size();
        anim->m_tracks = new FBXTrack[ anim->m_trackCount ];
        for (unsigned int track = 0 ; track < anim->m_trackCount ; ++track )
        {
            anim->m_tracks[track].m_boneIndex = tracks[track];
            anim->m_tracks[track].m_keyframeCount = frameCount;
            anim->m_tracks[track].m_keyframes = new FBXKeyFrame[ frameCount ];
        }

        // evaluate all of the animated track keyframes
        // loop by frame first to limit FBX time changes (dreadfully slow!)
        for ( unsigned int frame = 0 ; frame < frameCount ; ++frame )
        {
            frameTime.SetFrame(frame + anim->m_startFrame);

            for (unsigned int track = 0 ; track < anim->m_trackCount ; ++track )
            {
                FbxAMatrix localMatrix = m_importAssistor->evaluator->GetNodeLocalTransform( (FbxNode*)nodes[ track ], frameTime );

                FbxQuaternion R = localMatrix.GetQ();
                FbxVector4 T = localMatrix.GetT();
                FbxVector4 S = localMatrix.GetS();

                anim->m_tracks[track].m_keyframes[ frame ].m_key = frame + anim->m_startFrame;
                anim->m_tracks[track].m_keyframes[ frame ].m_rotation = glm::quat((float)R[3],(float)R[0],(float)R[1],(float)R[2]);
                anim->m_tracks[track].m_keyframes[ frame ].m_translation = glm::vec3((float)T[0],(float)T[1],(float)T[2]);
                anim->m_tracks[track].m_keyframes[ frame ].m_scale = glm::vec3((float)S[0],(float)S[1],(float)S[2]);
            }
        }

        m_animations[ anim->m_name ] = anim;
    }
}

void FBXFile::extractAnimationTrack(std::vector<int>& a_tracks, void* a_layer, void* a_node, std::vector<void*>& a_nodes, unsigned int& a_startFrame, unsigned int& a_endFrame)
{
    FbxAnimLayer* fbxAnimLayer = (FbxAnimLayer*)a_layer;
    FbxNode* fbxNode = (FbxNode*)a_node;

    // find node index in skeleton
    int boneIndex = -1;
    FBXSkeleton* skeleton = m_skeletons[0];
    for ( unsigned int i = 0 ; i < skeleton->m_boneCount ; ++i )
    {
        if (skeleton->m_nodes[i]->m_name == fbxNode->GetName())
        {
            boneIndex = i;
            break;
        }
    }

    // extract bones that have animated properties on them only
    bool hasKeys = false;
    FbxAnimCurve* lAnimCurve = nullptr;
    if (boneIndex >= 0)
    {
        // translation
        lAnimCurve = fbxNode->LclTranslation.GetCurve(fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
        if (lAnimCurve)
        {
            hasKeys = true;

            unsigned int frame = (unsigned int)lAnimCurve->KeyGetTime(0).GetFrameCount();
            if (frame < a_startFrame)
                a_startFrame = frame;

            frame = (unsigned int)lAnimCurve->KeyGetTime(lAnimCurve->KeyGetCount()-1).GetFrameCount();
            if (frame > a_endFrame)
                a_endFrame = frame;
        }
        lAnimCurve = fbxNode->LclTranslation.GetCurve(fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
        if (lAnimCurve)
        {
            hasKeys = true;

            unsigned int frame = (unsigned int)lAnimCurve->KeyGetTime(0).GetFrameCount();
            if (frame < a_startFrame)
                a_startFrame = frame;

            frame = (unsigned int)lAnimCurve->KeyGetTime(lAnimCurve->KeyGetCount()-1).GetFrameCount();
            if (frame > a_endFrame)
                a_endFrame = frame;

        }
        lAnimCurve = fbxNode->LclTranslation.GetCurve(fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
        if (lAnimCurve)
        {
            hasKeys = true;

            unsigned int frame = (unsigned int)lAnimCurve->KeyGetTime(0).GetFrameCount();
            if (frame < a_startFrame)
                a_startFrame = frame;

            frame = (unsigned int)lAnimCurve->KeyGetTime(lAnimCurve->KeyGetCount()-1).GetFrameCount();
            if (frame > a_endFrame)
                a_endFrame = frame;

        }

        // rotation
        lAnimCurve = fbxNode->LclRotation.GetCurve(fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
        if (lAnimCurve)
        {
            hasKeys = true;

            unsigned int frame = (unsigned int)lAnimCurve->KeyGetTime(0).GetFrameCount();
            if (frame < a_startFrame)
                a_startFrame = frame;

            frame = (unsigned int)lAnimCurve->KeyGetTime(lAnimCurve->KeyGetCount()-1).GetFrameCount();
            if (frame > a_endFrame)
                a_endFrame = frame;

        }
        lAnimCurve = fbxNode->LclRotation.GetCurve(fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
        if (lAnimCurve)
        {
            hasKeys = true;

            unsigned int frame = (unsigned int)lAnimCurve->KeyGetTime(0).GetFrameCount();
            if (frame < a_startFrame)
                a_startFrame = frame;

            frame = (unsigned int)lAnimCurve->KeyGetTime(lAnimCurve->KeyGetCount()-1).GetFrameCount();
            if (frame > a_endFrame)
                a_endFrame = frame;

        }
        lAnimCurve = fbxNode->LclRotation.GetCurve(fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
        if (lAnimCurve)
        {
            hasKeys = true;

            unsigned int frame = (unsigned int)lAnimCurve->KeyGetTime(0).GetFrameCount();
            if (frame < a_startFrame)
                a_startFrame = frame;

            frame = (unsigned int)lAnimCurve->KeyGetTime(lAnimCurve->KeyGetCount()-1).GetFrameCount();
            if (frame > a_endFrame)
                a_endFrame = frame;

        }

        // scale
        lAnimCurve = fbxNode->LclScaling.GetCurve(fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
        if (lAnimCurve)
        {
            hasKeys = true;

            unsigned int frame = (unsigned int)lAnimCurve->KeyGetTime(0).GetFrameCount();
            if (frame < a_startFrame)
                a_startFrame = frame;

            frame = (unsigned int)lAnimCurve->KeyGetTime(lAnimCurve->KeyGetCount()-1).GetFrameCount();
            if (frame > a_endFrame)
                a_endFrame = frame;

        }
        lAnimCurve = fbxNode->LclScaling.GetCurve(fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
        if (lAnimCurve)
        {
            hasKeys = true;

            unsigned int frame = (unsigned int)lAnimCurve->KeyGetTime(0).GetFrameCount();
            if (frame < a_startFrame)
                a_startFrame = frame;

            frame = (unsigned int)lAnimCurve->KeyGetTime(lAnimCurve->KeyGetCount()-1).GetFrameCount();
            if (frame > a_endFrame)
                a_endFrame = frame;

        }
        lAnimCurve = fbxNode->LclScaling.GetCurve(fbxAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
        if (lAnimCurve)
        {
            hasKeys = true;

            unsigned int frame = (unsigned int)lAnimCurve->KeyGetTime(0).GetFrameCount();
            if (frame < a_startFrame)
                a_startFrame = frame;

            frame = (unsigned int)lAnimCurve->KeyGetTime(lAnimCurve->KeyGetCount()-1).GetFrameCount();
            if (frame > a_endFrame)
                a_endFrame = frame;
        }

        if (hasKeys)
        {
            a_nodes.push_back(fbxNode);
            a_tracks.push_back(boneIndex);
        }
    }

    for (int i = 0; i < fbxNode->GetChildCount(); ++i)
    {
        extractAnimationTrack(a_tracks, fbxAnimLayer, fbxNode->GetChild(i), a_nodes, a_startFrame, a_endFrame);
    }
}

void FBXFile::extractSkeleton(FBXSkeleton* a_skeleton, void* a_scene)
{
    FbxScene* fbxScene = (FbxScene*)a_scene;

    int poseCount = fbxScene->GetPoseCount();

    for (int i = 0; i < poseCount; ++i)
    {
        FbxPose* lPose = fbxScene->GetPose(i);

        for ( int j = 0 ; j < lPose->GetCount() ; ++j )
        {
            float scaleFactor = m_importAssistor->unitScale;

            FbxMatrix scale(scaleFactor,0,0,0,
                            0,scaleFactor,0,0,
                            0,0,scaleFactor,0,
                            0,0,0,1);

            FbxMatrix lMatrix = lPose->GetMatrix(j);
            FbxMatrix lBindMatrix = lMatrix.Inverse() * scale;

            for ( unsigned int k = 0 ; k < a_skeleton->m_boneCount ; ++k )
            {
                if (a_skeleton->m_nodes[k]->m_name == lPose->GetNodeName(j).GetCurrentName())
                {
                    FbxVector4 row0 = lBindMatrix.GetRow(0);
                    FbxVector4 row1 = lBindMatrix.GetRow(1);
                    FbxVector4 row2 = lBindMatrix.GetRow(2);
                    FbxVector4 row3 = lBindMatrix.GetRow(3);

                    a_skeleton->m_bindPoses[ k ][0] = glm::vec4( (float)row0[0], (float)row0[1], (float)row0[2], (float)row0[3] );
                    a_skeleton->m_bindPoses[ k ][1] = glm::vec4( (float)row1[0], (float)row1[1], (float)row1[2], (float)row1[3] );
                    a_skeleton->m_bindPoses[ k ][2] = glm::vec4( (float)row2[0], (float)row2[1], (float)row2[2], (float)row2[3] );
                    a_skeleton->m_bindPoses[ k ][3] = glm::vec4( (float)row3[0], (float)row3[1], (float)row3[2], (float)row3[3] );
                }
            }
        }
    }
}

void FBXSkeleton::evaluate(const FBXAnimation* a_animation, float a_time, bool a_loop, float a_FPS)
{
    if (a_animation != nullptr)
    {
        // determine frame we're on
        int totalFrames = a_animation->m_endFrame - a_animation->m_startFrame;
        float animDuration = totalFrames / a_FPS;

        // get time through frame
        float frameTime = 0;
        if (a_loop)
            frameTime = glm::max(glm::mod(a_time,animDuration),0.0f);
        else
            frameTime = glm::min(glm::max(a_time,0.0f),animDuration);

        const FBXTrack* track = nullptr;
        const FBXKeyFrame* start = nullptr;
        const FBXKeyFrame* end = nullptr;

        for ( unsigned int i = 0 ; i < a_animation->m_trackCount ; ++i )
        {
            track = &(a_animation->m_tracks[i]);

            start = nullptr;
            end = nullptr;
            float startTime, endTime;

            // determine the two keyframes we're between
            for ( unsigned int j = 0 ; j < track->m_keyframeCount - 1 ; ++j )
            {
                startTime = (track->m_keyframes[j].m_key - a_animation->m_startFrame) / a_FPS;
                endTime = (track->m_keyframes[j + 1].m_key - a_animation->m_startFrame) / a_FPS;

                if (startTime <= frameTime &&
                    endTime >= frameTime)
                {
                    // found?
                    start = &(track->m_keyframes[j]);
                    end = &(track->m_keyframes[j+1]);
                    break;
                }
            }

            // interpolate between them
            if (start != nullptr &&
                end != nullptr)
            {
                float fScale = glm::max(0.0f,glm::min(1.0f,(frameTime - startTime) / (endTime - startTime)));

                // translation
                glm::vec3 T = glm::mix(start->m_translation,end->m_translation,fScale);

                // scale
                glm::vec3 S = glm::mix(start->m_scale,end->m_scale,fScale);

                // rotation (quaternion slerp)
                glm::quat R = glm::normalize(glm::slerp(start->m_rotation,end->m_rotation,fScale));

                // build matrix
                glm::mat4 mRot = glm::mat4_cast( R );
                glm::mat4 mScale = glm::scale( S );
                glm::mat4 mTranslate = glm::translate( T );
                m_nodes[ track->m_boneIndex ]->m_localTransform = mTranslate * mScale * mRot;
            }
        }
    }
}

void FBXSkeleton::updateBones()
{
    // update bones
    for ( int i = 0 ; i < (int)m_boneCount ; ++i )
    {
        if ( m_parentIndex[i] == -1 )
            m_bones[ i ] = m_nodes[ i ]->m_localTransform;
        else
            m_bones[i] = m_bones[ m_parentIndex[ i ] ] * m_nodes[ i ]->m_localTransform;
    }
    // combine bind pose
    for ( int i = 0 ; i < (int)m_boneCount ; ++i )
    {
        m_bones[ i ] = m_bones[ i ] * m_bindPoses[ i ];
    }
}

void FBXFile::calculateTangentsBinormals(std::vector<Vertex_FBX>& a_vertices, const std::vector<unsigned int>& a_indices)
{
    unsigned int vertexCount = (unsigned int)a_vertices.size();
    glm::vec3* tan1 = new glm::vec3[vertexCount * 2];
    glm::vec3* tan2 = tan1 + vertexCount;
    memset(tan1, 0, vertexCount * sizeof(glm::vec3) * 2);

    unsigned int indexCount = (unsigned int)a_indices.size();
    for (unsigned int a = 0; a < indexCount; a += 3)
    {
        unsigned int i1 = a_indices[a];
        unsigned int i2 = a_indices[a + 1];
        unsigned int i3 = a_indices[a + 2];

        const glm::vec4& v1 = a_vertices[i1].position;
        const glm::vec4& v2 = a_vertices[i2].position;
        const glm::vec4& v3 = a_vertices[i3].position;

        const glm::vec2& w1 = a_vertices[i1].texCoord1;
        const glm::vec2& w2 = a_vertices[i2].texCoord1;
        const glm::vec2& w3 = a_vertices[i3].texCoord1;

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

    for (unsigned int a = 0; a < vertexCount; a++)
    {
        const glm::vec3& n = a_vertices[a].normal.xyz();
        const glm::vec3& t = tan1[a];

        // Gram-Schmidt orthogonalise
        glm::vec3 p = t - n * glm::dot(n, t);
        if ( glm::length2(p) != 0 )
        {
            a_vertices[a].tangent = glm::vec4( glm::normalize( p ), 0.0f );

            // calculate binormal
            float sign = glm::dot(glm::cross(n.xyz(), t.xyz()), tan2[a].xyz()) < 0.0f ? -1.0f : 1.0f;
            a_vertices[a].binormal = glm::vec4(glm::cross(a_vertices[a].normal.xyz(),a_vertices[a].tangent.xyz()) * sign, 0);
        }
    }

    delete[] tan1;
}

unsigned int FBXFile::nodeCount(FBXNode* a_node)
{
    if (a_node == nullptr)
        return 0;

    unsigned int uiCount = 1;

    for (auto n : a_node->m_children)
        uiCount += nodeCount(n);

    return uiCount;
}

FBXMeshNode* FBXFile::getMeshByName(const char* a_name)
{
    for (auto mesh : m_meshes)
        if (mesh->m_name == a_name) return mesh;
    return nullptr;
}

FBXLightNode* FBXFile::getLightByName(const char* a_name)
{
    auto oIter = m_lights.find(a_name);
    if (oIter != m_lights.end())
        return oIter->second;
    return nullptr;
}

FBXCameraNode* FBXFile::getCameraByName(const char* a_name)
{
    auto oIter = m_cameras.find(a_name);
    if (oIter != m_cameras.end())
        return oIter->second;
    return nullptr;
}

Material* FBXFile::getMaterialByName(const char* a_name)
{
    auto oIter = m_materials.find(a_name);
    if (oIter != m_materials.end())
        return oIter->second;
    return nullptr;
}

const Texture& FBXFile::getTextureByName(const char* a_name) const
{
    auto oIter = m_textures.find(a_name);
    if (oIter != m_textures.end())
        return oIter->second;
    return Texture::Invalid;
}

FBXAnimation* FBXFile::getAnimationByName(const char* a_name)
{
    auto oIter = m_animations.find(a_name);
    if (oIter != m_animations.end())
        return oIter->second;
    return nullptr;
}

FBXLightNode* FBXFile::getLightByIndex(unsigned int a_index)
{
    for (auto t : m_lights)
    {
        if (a_index-- == 0)
            return t.second;
    }

    return nullptr;
}

FBXCameraNode* FBXFile::getCameraByIndex(unsigned int a_index)
{
    for (auto t : m_cameras)
    {
        if (a_index-- == 0)
            return t.second;
    }

    return nullptr;
}

Material* FBXFile::getMaterialByIndex(unsigned int a_index)
{
    for (auto t : m_materials)
    {
        if (a_index-- == 0)
            return t.second;
    }

    return nullptr;
}

FBXAnimation* FBXFile::getAnimationByIndex(unsigned int a_index)
{
    for (auto t : m_animations)
    {
        if (a_index-- == 0)
            return t.second;
    }

    return nullptr;
}

const Texture& FBXFile::getTextureByIndex(unsigned int a_index) const
{
    for(const auto& t : m_textures)
    {
        if (a_index-- == 0)
            return t.second;
    }

    return Texture::Invalid;
}

void FBXFile::gatherBones(void* a_object)
{
    FbxNode* fbxNode = (FbxNode*)a_object;

    if (fbxNode->GetNodeAttribute() != nullptr &&
        fbxNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
    {
        unsigned int index = (unsigned int)m_importAssistor->boneIndexList.size();
        m_importAssistor->boneIndexList[ fbxNode->GetName() ] = index;
    }

    for (int i = 0; i < fbxNode->GetChildCount(); i++)
    {
        gatherBones((void*)fbxNode->GetChild(i));
    }
}

void FBXNode::updateGlobalTransform()
{
    if (m_parent != nullptr)
        m_globalTransform = m_parent->m_globalTransform * m_localTransform;
    else
        m_globalTransform = m_localTransform;

    for (auto child : m_children)
        child->updateGlobalTransform();
}

void FBXCameraNode::updateGlobalTransform()
{
    if (m_parent != nullptr)
        m_globalTransform = m_parent->m_globalTransform * m_localTransform;
    else
        m_globalTransform = m_localTransform;

    m_viewMatrix = glm::inverse( m_globalTransform );

    for (auto child : m_children)
        child->updateGlobalTransform();
}

FBXAnimation* FBXAnimation::clone() const
{
    FBXAnimation* copy = new FBXAnimation();
    copy->m_name = m_name;
    copy->m_startFrame = m_startFrame;
    copy->m_endFrame = m_endFrame;
    copy->m_trackCount = m_trackCount;
    copy->m_tracks = new FBXTrack[ m_trackCount ];

    for ( unsigned int i = 0 ; i < m_trackCount ; ++i )
    {
        copy->m_tracks[i].m_boneIndex = m_tracks[i].m_boneIndex;
        copy->m_tracks[i].m_keyframeCount = m_tracks[i].m_keyframeCount;
        copy->m_tracks[i].m_keyframes = new FBXKeyFrame[ m_tracks[i].m_keyframeCount ];

        memcpy(copy->m_tracks[i].m_keyframes,m_tracks[i].m_keyframes,sizeof(FBXKeyFrame) * m_tracks[i].m_keyframeCount);
    }

    return copy;
}
