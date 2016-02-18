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
	FBXFile m_fbxFile; // Loads the FBX and stores the associated data.
	Program m_program;
    std::string m_meshFileName;

    // These two vectors match. Use the same index to grab the mesh and its diffuse texture
	std::vector<GLuint> m_diffuseTextureIds; // The OpenGL texture id
	std::vector<Mesh> m_meshes;

	float m_elapsedTime; // Time passed since creation.
};

