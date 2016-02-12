#pragma once

#include "Mesh.h"
#include "Program.h"
#include "Transform.h"
#include "Texture.h"

#include "FBXFile.h"

#include <vector>

/*
	A class that loads and renders a FBX model.
*/
class FBXMesh
{
public:
	bool create( const glm::vec3& pos, const char* pMeshFilename );
	void destroy();

	void update(float deltaTime);

	void draw(const glm::mat4& projectionViewMatrix);

private:
	FBXFile m_fbxFile;
	Program m_program;
	Transform m_transform;

	std::vector<GLuint> m_diffuseTextureIds;
	std::vector<Mesh> m_meshes;

	float m_elapsedTime;
};

