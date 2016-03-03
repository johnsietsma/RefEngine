#pragma once

#include "Engine/Component.h"
#include "Engine/Program.h"
#include "Engine/Renderable.h"
#include "Engine/BoundingVolume.h"
#include "Engine/Transform.h"

#include <memory>

class Camera;
class Light;
struct Renderable;

class GameObject
{
public:
    const Transform& getTransform() const 
    { 
        return m_transform;
    }

    void setTransform(const Transform& transform) { m_transform = transform; }

    virtual bool create() = 0;
    virtual void destroy();

    virtual void update(float deltaTime) {}; //no-op

    void draw(const Camera& camera, const Light& light, Program overrideProgram);

    void addComponent( std::shared_ptr<Component> component ) { m_components.push_back(component); }

    std::vector<std::shared_ptr<Component>>& getComponents() { return m_components; }
    const std::vector<std::shared_ptr<Component>>& getComponents() const { return m_components; }

    std::vector<Renderable>& getRenderables() { return m_renderables; }
    const std::vector<Renderable>& getRenderables() const { return m_renderables; }

    size_t getLayer() const { return m_layer; }

protected:
    GameObject() : GameObject(Transform()) {}; // GameObject can only be inherited

    GameObject(const Transform& transform, size_t layer = 0) :
        m_transform(transform),
        m_layer(layer)
    {
    }

    // Enables child classes to update render state before drawing.
    virtual void preDraw(const Camera& camera, const Light& light) {}; // no-op

    std::vector<Renderable> m_renderables;

    BoundingVolume m_boundingVolume;

private:
    Transform m_transform;
    std::vector<std::shared_ptr<Component>> m_components;
    size_t m_layer = 0;
};
