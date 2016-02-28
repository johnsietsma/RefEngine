#pragma once

#include "Transform.h"

#include <glm/vec3.hpp>

class Light
{
public:
    Light( const Transform& transform ) :
        m_transform(transform),
        m_color(1)
    {}

    const Transform& getTransform() const { return m_transform; }
    Transform& getTransform() { return m_transform; }

    const glm::vec3& getColor() const { return m_color; }

private:
    Transform m_transform;
    glm::vec3 m_color;
};
