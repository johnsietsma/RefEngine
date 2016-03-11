#pragma once

#include "engine/GameObject.h"
#include "graphics/Program.h"

#include "fbx/FBXFile.h"

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
    FBXMeshGameObject(const Transform& transform, const char* pMeshFilename, const char* pSkinnedMeshName=nullptr);

    bool create() override;
    void destroy() override;

    void update(float deltaTime) override;
    void preDraw(const CameraComponent& camera, const Light& light) override;


private:
    FBXFile m_fbxFile; // Loads the FBX and stores the associated data.
    std::string m_meshFileName;
    std::string m_skinnedMeshName;
    
    Program m_programs[4];

    float m_elapsedTime = 0; // Time passed since creation.
};

