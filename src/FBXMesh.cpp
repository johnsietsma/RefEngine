#include "FBXmesh.h"

#include "gl_core_4_4.h"
#include "ResourceCreator.h"

#include <assert.h>

bool FBXMesh::create(const glm::vec3& pos, const char* pMeshFilename)
{
	m_transform.Translate(pos);
	m_transform.SetScale(glm::vec3(0.001f, 0.001f, 0.001f));


	m_program = ResourceCreator::CreateProgram("./data/shaders/skinning.vert", "./data/shaders/tex.frag");
	if (!m_program.isValid()) return false;


	// ---- Create the quad geo ----
	if (!m_fbxFile.load(pMeshFilename)) return false;
	m_fbxFile.initialiseOpenGLTextures();


	for (unsigned int meshIndex = 0; meshIndex < m_fbxFile.getMeshCount(); meshIndex++)
	{
		auto pFbxMesh = m_fbxFile.getMeshByIndex(meshIndex);
		m_meshes.emplace_back();

		auto& mesh = m_meshes.back();
		mesh.create(pFbxMesh->m_vertices.data(), pFbxMesh->m_vertices.size(), pFbxMesh->m_indices.data(), pFbxMesh->m_indices.size());

		auto& pFbxDiffuseTexture = pFbxMesh->m_material->textures[FBXMaterial::DiffuseTexture];
		if (pFbxDiffuseTexture != nullptr) {
			m_diffuseTextureIds.emplace_back(pFbxDiffuseTexture->handle);

			// Bind the texture to texture unit 0
			glUseProgram(m_program.getId());
			m_program.setUniform("diffuseSampler", 0);

		}
		else {
			m_diffuseTextureIds.emplace_back(-1);
		}
	}

	return true;
}


void FBXMesh::destroy()
{
	m_fbxFile.unload();
	m_program.destroy();
	for (auto& mesh : m_meshes) {
		mesh.destroy();
	}
	m_meshes.clear();
	m_diffuseTextureIds.clear();
}


void FBXMesh::update(float deltaTime)
{
	if (m_fbxFile.getSkeletonCount() == 0 || m_fbxFile.getAnimationCount() == 0) return;

	m_elapsedTime += deltaTime;

	FBXSkeleton* pSkeleton = m_fbxFile.getSkeletonByIndex(0);
	FBXAnimation* pAnimation = m_fbxFile.getAnimationByIndex(0);
	assert(pAnimation->totalFrames() > 0);

	pSkeleton->evaluate(pAnimation, m_elapsedTime);

	for (unsigned int boneIndex = 0; boneIndex < pSkeleton->m_boneCount; boneIndex++) 
	{
		pSkeleton->m_nodes[boneIndex]->updateGlobalTransform();
	}

}

void FBXMesh::draw( const glm::mat4& projectionViewMatrix )
{
	// Use the program
	assert(m_program.isValid());
	glUseProgram(m_program.getId());
	m_program.setUniform("projectionView", projectionViewMatrix * m_transform.GetMatrix());

	// One diffuse texture for each mesh
	assert(m_meshes.size() == m_diffuseTextureIds.size());

	for (unsigned int meshIndex = 0; meshIndex < m_meshes.size(); meshIndex++ )
	{
		auto& mesh = m_meshes[meshIndex];
		// Bind the mesh
		glBindVertexArray(mesh.getVAO());

		GLuint textureId = m_diffuseTextureIds[meshIndex];

		if (textureId != (GLuint)-1) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureId);

		}

		if (m_fbxFile.getSkeletonCount() > 0)
		{
			FBXSkeleton* pSkeleton = m_fbxFile.getSkeletonByIndex(0);
			pSkeleton->updateBones();

			m_program.setUniform("bones", *pSkeleton->m_bones, pSkeleton->m_boneCount);

		}

		glDrawElements(GL_TRIANGLES, mesh.getIndexCount(), GL_UNSIGNED_INT, 0);
	}

}