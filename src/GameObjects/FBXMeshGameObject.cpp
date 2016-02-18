#include "FBXMeshGameObject.h"

#include "Engine\ResourceCreator.h"

#include "gl_core_4_4.h"

#include <assert.h>

FBXMeshGameObject::FBXMeshGameObject(const Transform& transform, const char* pMeshFilename) :
    GameObject(transform),
    m_meshFileName(pMeshFilename)
{}


bool FBXMeshGameObject::create()
{
    // Create the program for rendering this FBX.
    // Use a skinning vertex shader to support animation.
    // Use the default textured fragment shader.
	m_program = ResourceCreator::CreateProgram("./data/shaders/skinning.vert", "./data/shaders/tex.frag");
	if (!m_program.isValid()) return false;

	if (!m_fbxFile.load(m_meshFileName.c_str())) return false;

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
		m_meshes.emplace_back(); // Creates a Mesh, calling the constructor with no parameters.
		auto& mesh = m_meshes.back(); // Get a reference to the new mesh.

        // Grab the vertex and index data and upload it to OpenGL
		mesh.create(pFbxMesh->m_vertices.data(), pFbxMesh->m_vertices.size(), pFbxMesh->m_indices.data(), pFbxMesh->m_indices.size());

        GLuint diffuseTextureId = -1;

        // A mesh can have multiple materials. For simplicity we'll only use the first one
		if (pFbxMesh->m_materials.size() > 0) {

            // We're only using the diffuse texture right now. Check to see if one has been loaded
            auto& pFbxDiffuseTexture = pFbxMesh->m_materials[0]->textures[FBXMaterial::DiffuseTexture];
            if (pFbxDiffuseTexture != nullptr) {
                // Found it!
                diffuseTextureId = pFbxDiffuseTexture->handle;

                // Bind the texture to texture unit 0
                m_program.setUniform("diffuseSampler", 0);
            }
		}

        // Store the texture id (even if it doesn't exist), so we match the mesh vector index.
		m_diffuseTextureIds.emplace_back(diffuseTextureId);
	}

	return true;
}


void FBXMeshGameObject::destroy()
{
	m_fbxFile.unload();

	m_program.destroy();
	for (auto& mesh : m_meshes) {
		mesh.destroy();
	}
	m_meshes.clear();
	m_diffuseTextureIds.clear();
}


void FBXMeshGameObject::update(float deltaTime)
{
	if (m_fbxFile.getSkeletonCount() == 0 || m_fbxFile.getAnimationCount() == 0) return;

    // Keep track of how mcuh time has passed
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

void FBXMeshGameObject::draw(const Camera& camera)
{
	// Use the program
	assert(m_program.isValid());
	glUseProgram(m_program.getId());

	m_program.setUniform("projectionView", camera.getProjectionView() * getTransform().GetMatrix());

	// Make sure we only have one diffuse texture for each mesh
	assert(m_meshes.size() == m_diffuseTextureIds.size());

	for (unsigned int meshIndex = 0; meshIndex < m_meshes.size(); meshIndex++ )
	{
        // Get the mesh we're rendering
		auto& mesh = m_meshes[meshIndex];

		// Bind the mesh
		glBindVertexArray(mesh.getVAO());

        // Grab the diffuse texture we're rendering with
		GLuint textureId = m_diffuseTextureIds[meshIndex];

		if (textureId != (GLuint)-1) {
            // Associate the diffuse texture id with texture unit 0
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureId);
		}

		if (m_fbxFile.getSkeletonCount() > 0)
		{
			FBXSkeleton* pSkeleton = m_fbxFile.getSkeletonByIndex(0);

            // Use the nodes we've evaluated above to update the bone positions and combine with the bind pose.
			pSkeleton->updateBones(); 

            // Set the bone data uniform in the vertex shader
			m_program.setUniform("bones", *pSkeleton->m_bones, pSkeleton->m_boneCount);

		}

		glDrawElements(GL_TRIANGLES, mesh.getIndexCount(), GL_UNSIGNED_INT, 0);
	}

}