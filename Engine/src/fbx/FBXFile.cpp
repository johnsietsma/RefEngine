#include "FBXFile.h"

#include "FBXNode.h"
#include "FBXMeshNode.h"
#include "FBXAnimation.h"

#include "data/Vertex.h"
#include "engine/Material.h"

#include "gl_core_4_4.h"

#include <fbxsdk.h>

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
    FbxScene*               scene;
    FbxAnimEvaluator*       evaluator;
    FbxImporter*            importer;
    std::vector<FBXNode*>   bones;
    std::map<std::string,int> boneIndexList;
    float unitScale;
};



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


    m_meshes.clear();
    m_lights.clear();
    m_cameras.clear();
    m_materials.clear();
    m_skeletons.clear();
    m_animations.clear();
}

bool FBXFile::load( const char* a_filename, FBXLoadConfig loadConfig )
{
    m_loadConfig = loadConfig;
    
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
    if ( lScene->GetGlobalSettings().GetSystemUnit() != FbxSystemUnit::sPredefinedUnits[(int)loadConfig.unitScale] )
    {
        const FbxSystemUnit::ConversionOptions lConversionOptions = {
            false, // mConvertRrsNodes
            true, // mConvertAllLimits
            true, // mConvertClusters
            true, // mConvertLightIntensity
            true, // mConvertPhotometricLProperties
            true  // mConvertCameraClipPlanes
        };

        unitScale = (float)(lScene->GetGlobalSettings().GetSystemUnit().GetScaleFactor() / FbxSystemUnit::sPredefinedUnits[(int)loadConfig.unitScale].GetScaleFactor());

        // Convert the scene to meters using the defined options.
        FbxSystemUnit::sPredefinedUnits[(int)loadConfig.unitScale].ConvertScene(lScene, lConversionOptions);
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
        m_importAssistor->unitScale = unitScale;

        m_root = new FBXNode();
        m_root->m_name = "root";
        m_root->m_globalTransform = m_root->m_localTransform = glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);

        // grab the ambient light data from the scene
        FbxColor ambientColor = lScene->GetGlobalSettings().GetAmbientColor();
        m_ambientLight.x = (float)ambientColor.mRed;
        m_ambientLight.y = (float)ambientColor.mGreen;
        m_ambientLight.z = (float)ambientColor.mBlue;
        m_ambientLight.w = (float)ambientColor.mAlpha;
        
        // gather bones to create indices for them in a skeleton
        for (unsigned int i = 0; i < (unsigned int)lNode->GetChildCount(); ++i)
        {
            gatherBones((void*)lNode->GetChild(i));
        }

        // extract scene (meshes, lights, cameras)
        for (i = 0; i < (unsigned int)lNode->GetChildCount(); ++i)
        {
            extractObject(m_root, (void*)lNode->GetChild(i));
        }

        if (m_loadConfig.loadAnimations) {
            extractBonesAndAnimations(lScene);
        }

        m_root->updateGlobalTransform();
        
        delete m_importAssistor;
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
                if (m_loadConfig.loadMeshes)
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
                if (m_loadConfig.loadCameras == false)
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
                if (m_loadConfig.loadLights == false)
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

    if (m_loadConfig.loadAnimations == true &&
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
    
    meshNode.LoadVertexPositions(pVertexPositions, vertexCount);
    meshNode.LoadVertexIndices(pFbxMesh);

    FbxGeometryElementVertexColor* fbxColours = pFbxMesh->GetElementVertexColor(0);
    if( fbxColours!=nullptr )
    {
        meshNode.LoadVertexColors( fbxColours, vertexCount );
    }
    
    FbxGeometryElementUV* fbxTexCoord0 = pFbxMesh->GetElementUV(0);
    if( fbxTexCoord0 )
    {
        meshNode.LoadTexCoords( fbxTexCoord0,  pFbxMesh, m_loadConfig.flipTextureY, 0);
    }

    FbxGeometryElementUV* fbxTexCoord1 = pFbxMesh->GetElementUV(1);
    if( fbxTexCoord1 )
    {
        meshNode.LoadTexCoords( fbxTexCoord1,  pFbxMesh, m_loadConfig.flipTextureY, 1);
    }

    FbxGeometryElementNormal* fbxNormal = pFbxMesh->GetElementNormal(0);
    if( fbxNormal )
    {
        meshNode.LoadNormals(fbxNormal, vertexCount);
    }
    
    // gather skinning info
    meshNode.LoadSkinningData(pFbxMesh, m_importAssistor->boneIndexList);

    // set mesh names, vertex attributes, extract material and add to mesh map
    for ( int i = 0 ; i < pFbxMesh->GetElementMaterialCount(); ++i )
    {
        meshNode.m_materials.push_back( extractMaterial(pFbxMesh,i) );
    }

    meshNode.calculateTangentsAndBinormals();
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

        if (m_loadConfig.loadTextures == true)
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

void FBXFile::extractBonesAndAnimations(void* a_scene)
{
    // build skeleton and extract animation keyframes
    if (m_importAssistor->bones.size() == 0) return;
    
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


unsigned int FBXFile::nodeCount(FBXNode* a_node)
{
    if (a_node == nullptr)
        return 0;

    unsigned int uiCount = 1;

    for (auto n : a_node->m_children)
        uiCount += nodeCount(n);

    return uiCount;
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
