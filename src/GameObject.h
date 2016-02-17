#pragma once

#include "Program.h"
#include "Transform.h"

class Camera;

class GameObject
{
public:
    GameObject(const Transform& transform) : 
        m_transform(transform)
    {
    }

    const Transform& getTransform() const { return m_transform;  }
    void setTransform(const Transform& transform) { m_transform = transform; }

    virtual bool create() = 0;
    virtual void destroy() = 0;

    virtual void update(float deltaTime) = 0;
    virtual void draw(const Camera& camera) = 0;

protected:
    GameObject() {}; // GameObject can only be inherited

private:
    Transform m_transform;
};
