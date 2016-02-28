#pragma once

#include "Engine/Component.h"
#include "Engine/Program.h"
#include "Engine/Renderable.h"
#include "Engine/BoundingVolume.h"
#include "Engine/Transform.h"
#include "Engine/Visitor.h"

#include <memory>

class Camera;
class Light;
struct Renderable;

class GameObject
{
public:

    const Transform& getTransform() const { return m_transform;  }
    void setTransform(const Transform& transform) { m_transform = transform; }

    virtual bool create() = 0;
    virtual void destroy() = 0;

    virtual void update(float deltaTime) {}; //no-op
    virtual void draw(const Camera& camera, const Light& light);

    template<typename... TArgs>
    Visitor<Renderable,TArgs...> getRenderablesVisitor() { return {m_renderables}; }

    void addComponent( std::shared_ptr<Component> component ) { m_components.push_back(component); }

    template<typename... TArgs>
    SharedPtrVisitor<Component,TArgs...> getComponentVisitor() { return {m_components}; }

protected:
    GameObject() : GameObject(Transform()) {}; // GameObject can only be inherited

    GameObject(const Transform& transform) :
        m_transform(transform)
    {
    }

    std::vector<Renderable> m_renderables;

    BoundingVolume m_boundingVolume;

private:
    Transform m_transform;
    std::vector< std::shared_ptr<Component> > m_components;
};
