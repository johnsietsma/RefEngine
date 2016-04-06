#pragma once

#include "Transform.h"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

struct CameraConfig
{
    CameraConfig( float a_fovY, float a_aspectRatio, float a_nearClip, float a_farClip ) :
        fovY( a_fovY ),
        aspectRatio( a_aspectRatio ),
        nearClip( a_nearClip ),
        farClip( a_farClip )
    {}
    
    float fovY;
    float aspectRatio;
    float nearClip;
    float farClip;
};

class Camera {
public:

    Camera(float a_fovY, float a_aspectRatio, float a_nearClip, float a_farClip);
    Camera( CameraConfig config );
    virtual ~Camera();

    void                setPerspective(float fovY, float aspectRatio, float nearClip, float farClip);

    const glm::mat4&    getProjectionTransform() const { return m_projection; }
    void                getFrustumPlanes( const glm::mat4& viewTransform, glm::vec4 (&pPlanes)[6] ) const;

private:
    glm::mat4   m_projection;
};
