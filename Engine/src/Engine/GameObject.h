#pragma once

#include "Engine/Program.h"
#include "Engine/Renderable.h"
#include "Engine/BoundingVolume.h"
#include "Engine/Transform.h"

#include <memory>

class Camera;

class GameObject
{
public:
  
    const Transform& getTransform() const { return m_transform;  }
    void setTransform(const Transform& transform) { m_transform = transform; }

    virtual bool create() = 0;
    virtual void destroy() = 0;

    virtual void update(float deltaTime) {};
    virtual void draw(const Camera& camera);

protected:
    GameObject() {}; // GameObject can only be inherited

    GameObject(const Transform& transform) :
        m_transform(transform)
    {
    }

    std::vector<Renderable> m_renderables;
    BoundingVolume m_boundingVolume;

private:
    Transform m_transform;
};
