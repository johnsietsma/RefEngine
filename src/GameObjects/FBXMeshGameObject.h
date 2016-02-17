#pragma once

#include "Engine/Camera.h"
#include "Engine/GameObject.h"
#include "Engine/Mesh.h"
#include "Engine/Program.h"
#include "Engine/Transform.h"
#include "Engine/Texture.h"

#include "FBXFile.h"

#include <string>
#include <vector>

/*
	A class that loads and renders a FBX model.
*/
class FBXMeshGameObject : public GameObject
{
public:
    FBXMeshGameObject(const Transform& transform, const char* pMeshFilename);

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

