#pragma once

#include "Camera.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Program.h"
#include "Transform.h"
#include "Texture.h"

#include "FBXFile.h"

#include <string>
#include <vector>

/*
	A class that loads and renders a FBX model.
*/
class FBXMesh : public GameObject
{
public:
    FBXMesh(const Transform& transform, const char* pMeshFilename);

	bool create() override;
	void destroy() override;

	void update(float deltaTime) override;

	void draw(const Camera& camera) override;

private:
	FBXFile m_fbxFile;
	Program m_program;
    std::string m_meshFileName;

	std::vector<GLuint> m_diffuseTextureIds;
	std::vector<Mesh> m_meshes;

	float m_elapsedTime;
};

