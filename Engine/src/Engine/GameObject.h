#pragma once

#include "engine/Component.h"
#include "engine/BoundingVolume.h"
#include "engine/Transform.h"

#include "graphics/Program.h"
#include "graphics/Renderable.h"

#include <memory>

class CameraComponent;
class Light;
struct Renderable;

class GameObject
{
public:
    virtual bool create() = 0;
    virtual void update(float deltaTime) {}; //no-op
    void draw(const CameraComponent& camera, const Light& light, const Program& overrideProgram);
    virtual void destroy();

    // ---- Getters ----
    const Transform& getTransform() const { return m_transform; }
    size_t getLayer() const { return m_layer; }

    std::vector<std::shared_ptr<Component>>& getComponents() { return m_components; }
    const std::vector<std::shared_ptr<Component>>& getComponents() const { return m_components; }

    std::vector<Renderable>& getRenderables() { return m_renderables; }
    const std::vector<Renderable>& getRenderables() const { return m_renderables; }


    // ---- Setters ----
    void setTransform(const Transform& transform) { m_transform = transform; }
    void addComponent(std::shared_ptr<Component> component) { m_components.push_back(component); }


    // ---- Input Events ----
    virtual void onKeyEvent(Input::Key key, Input::Action action);
    virtual void onMouseButton(Input::MouseButton button, Input::Action action);

protected:
    GameObject() : GameObject(Transform()) {}; // GameObject can only be inherited

    GameObject(const Transform& transform, size_t layer = 0) :
        m_transform(transform),
        m_layer(layer)
    {
    }

    // Enables child classes to update render state before drawing.
    virtual void preDraw(const CameraComponent& camera, const Light& light) {}; // no-op

    std::vector<Renderable> m_renderables;
    BoundingVolume m_boundingVolume;

private:
    Transform m_transform;
    std::vector<std::shared_ptr<Component>> m_components;
    size_t m_layer = 0;
};
