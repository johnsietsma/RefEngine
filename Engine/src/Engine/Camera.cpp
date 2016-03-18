
#include "engine/Camera.h"

#include <glm/gtx/quaternion.hpp>

using glm::vec3;
using glm::vec4;

Camera::Camera( CameraConfig config )
{
    setPerspective(config.fovY, config.aspectRatio, config.nearClip, config.farClip);
}

Camera::~Camera()
{

}


void Camera::getFrustumPlanes(const glm::mat4& viewTransform, glm::vec4 (&pPlanes)[6] ) const
{
    glm::mat4 transform = m_projection * viewTransform;

    // right side
    pPlanes[0] = vec4(  transform[0][3] - transform[0][0],
                        transform[1][3] - transform[1][0],
                        transform[2][3] - transform[2][0],
                        transform[3][3] - transform[3][0]);
    // left side
    pPlanes[1] = vec4(  transform[0][3] + transform[0][0],
                        transform[1][3] + transform[1][0],
                        transform[2][3] + transform[2][0],
                        transform[3][3] + transform[3][0]);
    // top
    pPlanes[2] = vec4(  transform[0][3] - transform[0][1],
                        transform[1][3] - transform[1][1],
                        transform[2][3] - transform[2][1],
                        transform[3][3] - transform[3][1]);
    // bottom
    pPlanes[3] = vec4(  transform[0][3] + transform[0][1],
                        transform[1][3] + transform[1][1],
                        transform[2][3] + transform[2][1],
                        transform[3][3] + transform[3][1]);
    // far
    pPlanes[4] = vec4(  transform[0][3] - transform[0][2],
                        transform[1][3] - transform[1][2],
                        transform[2][3] - transform[2][2],
                        transform[3][3] - transform[3][2]);
    // near
    pPlanes[5] = vec4(  transform[0][3] + transform[0][2],
                        transform[1][3] + transform[1][2],
                        transform[2][3] + transform[2][2],
                        transform[3][3] + transform[3][2]);

    for (int i = 0; i < 6; i++) {
        float mag = glm::length(vec3(pPlanes[i]));
        pPlanes[i] = pPlanes[i] / mag;
    }
}

void Camera::setPerspective(float fovY, float aspectRatio, float near, float far)
{
    m_projection = glm::perspective(fovY, aspectRatio, near, far);
}

