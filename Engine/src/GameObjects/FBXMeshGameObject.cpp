#include "FBXMeshGameObject.h"

#include "Engine/Camera.h"
#include "Engine/Material.h"
#include "Engine/ResourceCreator.h"
#include "Engine/Sampler.h"
#include "Engine/Texture.h"

#include "File/FBXMeshNode.h"
#include "File/FBXAnimation.h"

#include "gl_core_4_4.h"

#include <assert.h>


FBXMeshGameObject::FBXMeshGameObject(const Transform& transform, const char* pMeshFilename, const char* pSkinnedMeshName) :
    GameObject(transform),
    m_meshFileName(pMeshFilename),
    m_skinnedMeshName(pSkinnedMeshName ? pSkinnedMeshName : "")
{}

bool FBXMeshGameObject::create()
{
    m_programs[0] = ResourceCreator::CreateProgram("normal", "vertexLit"); // No anims, not texture
    m_programs[1] = ResourceCreator::CreateProgram("texturedNormal", "texturedVertexLit"); // No anims, textured
    m_programs[2] = ResourceCreator::CreateProgram("skinning", "vertexLit"); // Anims, not textured
    m_programs[3] = ResourceCreator::CreateProgram("skinning", "texturedVertLit"); // Anims, textured
    
    for( auto& prog : m_programs ) {
        if( !prog.isValid() )
            return false;
    }
    
    if (!m_fbxFile.load(m_meshFileName.c_str()))
        return false;

    // Extract the mesh and diffuse textures
    for (const auto pFbxMesh : m_fbxFile.getMeshes())
    {
        // Add a new mesh to our collection
        m_renderables.emplace_back(); // Creates a Mesh, calling the constructor with no parameters.
        auto& renderable = m_renderables.back(); // Get a reference to the new mesh.
        
        const Vertices_FBX& vertices = pFbxMesh->m_vertices;
        std::vector<VertexDataBuffer> fbxVertexDataBuffers {
            vertices.position,
            vertices.color,
            vertices.normal,
            vertices.tangent,
            vertices.binormal,
            vertices.indices,
            vertices.weights,
            vertices.texCoord1,
            vertices.texCoord2
        };

        // Grab the vertex and index data and upload it to OpenGL
        renderable.mesh.create( fbxVertexDataBuffers, pFbxMesh->m_indices.data(), (GLsizei)pFbxMesh->m_indices.size() );

        // Put a bounding volume around it for frustum culling
        m_boundingVolume.addBoundingSphere(glm::vec3(pFbxMesh->m_globalTransform[3]), pFbxMesh->m_vertices.position);

        // Figure out how to index into the the 4 programs created above
        int texIndex  = 0;
        int animIndex = pFbxMesh->m_name == m_skinnedMeshName ? 2 : 0;

        // A mesh can have multiple materials. For simplicity we'll only use the first one
        if (pFbxMesh->m_materials.size() > 0) {
            Material* pMaterial = pFbxMesh->m_materials[0];

            glUseProgram(renderable.program.getId());

            for (int textureIndex = 0; textureIndex < (size_t)Material::TextureType::Count; textureIndex++) {
                auto& texturePath = pMaterial->texturePaths[textureIndex];
                Texture texture = ResourceCreator::CreateTexture( texturePath.c_str() );

                if (texture.isValid()) {
                    texIndex = 1;
                    
                    // Bind the texture to a texture unit. textureIndex _must_ be an int.
                    renderable.program.setUniform(Material::getTextureUniformName((Material::TextureType)textureIndex), textureIndex);

                    renderable.samplers.emplace_back(
                            Texture(texture.getId()),
                            textureIndex // Use the index as the texture unit
                        );
                }
            }
        }
        
        // Take a copy of the correct program. This class does the cleanup.
        renderable.program = m_programs[texIndex + animIndex]; 
    }


    return true;
}


void FBXMeshGameObject::destroy()
{
    m_fbxFile.unload(); // Clean up texture resources.

    for( auto& prog: m_programs ) 
    {
        prog.destroy();
    }

    GameObject::destroy();
}


void FBXMeshGameObject::update(float deltaTime)
{
    // Keep track of how much time has passed
    m_elapsedTime += deltaTime;
    
    std::vector<FBXSkeleton*>& skeletons = m_fbxFile.getSkeletons();
    auto& animationMap = m_fbxFile.getAnimations();
    
    if( skeletons.size() == 0 || animationMap.size() == 0  )
        return;

    // Just use the first skeleton and first animation.
    FBXSkeleton* pSkeleton = skeletons[0];
    const FBXAnimation* pAnimation = animationMap.begin()->second;
    assert(pAnimation->totalFrames() > 0);

    // Get the position, scale, rotation values of the keyframes in the animation at m_elapsedTime.
    // Interpolate between the two closest keyframes and set the local transform of animation nodes.
    pSkeleton->evaluate(pAnimation, m_elapsedTime);

    for (unsigned int boneIndex = 0; boneIndex < pSkeleton->m_boneCount; boneIndex++)
    {
        // Refresh the global transform now that we've updated the local transform
        pSkeleton->m_nodes[boneIndex]->updateGlobalTransform();
    }

}

void FBXMeshGameObject::preDraw(const Camera& camera, const Light& light)
{
    for( auto& renderable : m_renderables )
    {
        Program& program = renderable.program;
        assert(program.isValid());
        glUseProgram(program.getId());
        
        std::vector<FBXSkeleton*>& skeletons = m_fbxFile.getSkeletons();

        if (skeletons.size() > 0 && program.hasUniform("bones"))
        {
            FBXSkeleton* pSkeleton = skeletons[0];

            // Use the nodes we've evaluated above to update the bone positions and combine with the bind pose.
            pSkeleton->updateBones();

            // Set the bone data uniform in the vertex shader
            program.setUniform("bones", *pSkeleton->m_bones, pSkeleton->m_boneCount);

        }
    }
}

