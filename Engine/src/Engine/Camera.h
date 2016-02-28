#pragma once

#include "Transform.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:

    Camera(const Transform& transform, float fovY, float aspectRatio, float near, float far);
    virtual ~Camera();

    void    update(float deltaTime);

    void    setSpeed(float speed) { m_speed = speed; }

    void    setPerspective(float fovY, float aspectRatio, float near, float far);

    const Transform&    getTransform() const { return m_transform; }
    const glm::mat4&    getProjection() const { return m_projection; }
    glm::mat4           getView() const { return glm::inverse(m_transform.getMatrix());  }
    glm::mat4           getProjectionView() const { return m_projection * getView(); }
    void                getFrustumPlanes( glm::vec4 (&pPlanes)[6] ) const;

    // returns a world-space normalized vector pointing away from the camera's world-space position
    glm::vec3  screenPositionToDirection(float x, float y) const;

    // returns the point of intersection of a camera ray and a world-space plane
    // the plane has a normal of XYZ and is offset from (0,0,0) by -W in the direction of the normal
    glm::vec3           pickAgainstPlane(float x, float y, const glm::vec4& plane) const;

private:

    Transform   m_transform;
    float       m_speed;
    glm::mat4   m_projection;
};
