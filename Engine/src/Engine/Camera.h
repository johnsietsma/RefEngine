#pragma once

#include "Transform.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

class Camera {
public:

    Camera(float fovY, float aspectRatio, float near, float far);
    virtual ~Camera();

    void                setPerspective(float fovY, float aspectRatio, float near, float far);

    const glm::mat4&    getProjectionTransform() const { return m_projection; }
    void                getFrustumPlanes( const glm::mat4& viewTransform, glm::vec4 (&pPlanes)[6] ) const;

private:
    glm::mat4   m_projection;
};
