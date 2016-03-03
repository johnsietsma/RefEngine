#pragma once

#include "Engine/GameObject.h"
#include "Engine/Program.h"

#include "FBXFile.h"

#include <string>
#include <vector>

class Camera;
class Transform;
/*
    A class that loads and renders a FBX model.
*/
class FBXMeshGameObject : public GameObject
{
public:
    FBXMeshGameObject(const Transform& transform, const char* pMeshFilename, const char* pSkinnedMeshName);

    bool create() override;
    void destroy() override;

    void update(float deltaTime) override;

    void draw(const Camera& camera, const Light& light) override;

private:
    FBXFile m_fbxFile; // Loads the FBX and stores the associated data.
    std::string m_meshFileName;
    std::string m_skinnedMeshName;

    Program m_defaultProgram;
    Program m_skinningProgram;

    float m_elapsedTime = 0; // Time passed since creation.
};

