#include "FBXMeshGameObject.h"

#include "Engine/Camera.h"
#include "Engine/ResourceCreator.h"
#include "Engine/Sampler.h"
#include "Engine/Texture.h"

#include "gl_core_4_4.h"

#include <assert.h>


FBXMeshGameObject::FBXMeshGameObject(const Transform& transform, const char* pMeshFilename, const char* pSkinnedMeshName) :
    GameObject(transform),
    m_meshFileName(pMeshFilename),
    m_skinnedMeshName(pSkinnedMeshName ? pSkinnedMeshName : "")
{}


bool FBXMeshGameObject::create()
{
    // Create the program for rendering the non-skinned meshes.
    m_defaultProgram = ResourceCreator::CreateProgram("./data/shaders/fragLit.vert", "./data/shaders/texturedVertLit.frag");
    if (!m_defaultProgram.isValid())
        return false;

    // Create the program for rendering this FBX.
    // Use a skinning vertex shader to support animation.
    // Use the default textured fragment shader.
    m_skinningProgram = ResourceCreator::CreateProgram("./data/shaders/skinningFragLit.vert", "./data/shaders/texturedVertLit.frag");
    if (!m_skinningProgram.isValid())
        return false;

    if (!m_fbxFile.load(m_meshFileName.c_str()))
        return false;

    // FBX files can have file paths to textures they use. When the FBX is loaded these textures will have been read from disk.
    // initialiseOpenGLTextures() creates OpenGL texture ids and uploads the texture data to OpenGL.
    // The texture id can be retrieved via m_fbxFile->getMeshByIndex(0)->m_materials[0]->textures[FBXMaterial::DiffuseTexture]->handle *phew!*
    // See below for details.
    m_fbxFile.initialiseOpenGLTextures();

    // Extract the mesh and diffuse textures
    for (unsigned int meshIndex = 0; meshIndex < m_fbxFile.getMeshCount(); meshIndex++)
    {
        auto pFbxMesh = m_fbxFile.getMeshByIndex(meshIndex);

        // Add a new mesh to our collection
        m_renderables.emplace_back(); // Creates a Mesh, calling the constructor with no parameters.
        auto& renderable = m_renderables.back(); // Get a reference to the new mesh.

        // Grab the vertex and index data and upload it to OpenGL
        renderable.mesh.create(pFbxMesh->m_vertices.data(), pFbxMesh->m_vertices.size(), pFbxMesh->m_indices.data(), pFbxMesh->m_indices.size());

        m_boundingVolume.addBoundingSphere(glm::vec3(pFbxMesh->m_globalTransform[3]), pFbxMesh->m_vertices);

        // Take a copy of the correct program.
        // This object still owns the resource and needs to clean it up.
        renderable.program = pFbxMesh->m_name == m_skinnedMeshName ? m_skinningProgram : m_defaultProgram;

        // A mesh can have multiple materials. For simplicity we'll only use the first one
        if (pFbxMesh->m_materials.size() > 0) {
            FBXMaterial* pMaterial = pFbxMesh->m_materials[0];

            glUseProgram(renderable.program.getId());

            for (unsigned int textureIndex = 0; textureIndex < FBXMaterial::TextureTypes_Count; textureIndex++) {
                auto& pTexture = pMaterial->textures[textureIndex];

                if (pTexture != nullptr) {
                    // Bind the texture to a texture unit
                    renderable.program.setUniform(FBXMaterial::getTextureName(textureIndex), textureIndex);

                    renderable.samplers.emplace_back(
                            Texture(pTexture->handle),
                            textureIndex // Use the indx as the texture unit
                        );
                }
            }
        }
    }


    return true;
}


void FBXMeshGameObject::destroy()
{
    m_fbxFile.unload(); // Clean up texture resources.

    m_defaultProgram.destroy();
    m_skinningProgram.destroy();

    for (auto& renderable : m_renderables) {
        renderable.mesh.destroy();
    }
    m_renderables.clear();
}


void FBXMeshGameObject::update(float deltaTime)
{
    if (m_fbxFile.getSkeletonCount() == 0 || m_fbxFile.getAnimationCount() == 0) return;

    // Keep track of how much time has passed
    m_elapsedTime += deltaTime;

    // Just use the first skeleton and first animation.
    FBXSkeleton* pSkeleton = m_fbxFile.getSkeletonByIndex(0);
    FBXAnimation* pAnimation = m_fbxFile.getAnimationByIndex(0);
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

void FBXMeshGameObject::draw(const Camera& camera, const Light& light)
{

    for (unsigned int renderableIndex = 0; renderableIndex < m_renderables.size(); renderableIndex++ )
    {
        auto& renderable = m_renderables[renderableIndex];

        Program& program = renderable.program;
        assert(program.isValid());
        glUseProgram(program.getId());

        if (m_fbxFile.getSkeletonCount() > 0 && program.hasUniform("bones"))
        {
            FBXSkeleton* pSkeleton = m_fbxFile.getSkeletonByIndex(0);

            // Use the nodes we've evaluated above to update the bone positions and combine with the bind pose.
            pSkeleton->updateBones();

            // Set the bone data uniform in the vertex shader
            program.setUniform("bones", *pSkeleton->m_bones, pSkeleton->m_boneCount);

        }
    }

    GameObject::draw(camera, light);

}
