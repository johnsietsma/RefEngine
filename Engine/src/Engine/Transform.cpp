#include "Transform.h"

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtx/transform.hpp"

const glm::vec3 Transform::WORLD_UP = glm::vec3(0,1,0);

Transform::Transform(glm::vec3 position)
: Transform(position, glm::quat(), glm::vec3(1) )
{
}


Transform::Transform(glm::vec3 position, glm::quat rot) :
Transform(position, rot, glm::vec3(1))
{
}

Transform::Transform(glm::vec3 position, glm::quat rot, glm::vec3 scale) :
	m_position(position),
	m_rotation(rot),
	m_scale(scale)
{
}

Transform::Transform(glm::vec3 position, glm::vec3 lookPosition) :
    Transform(position)
{
    lookAt(lookPosition);
}


glm::mat4 Transform::getMatrix() const
{
	return glm::translate(m_position) * glm::mat4_cast(m_rotation) * glm::scale(m_scale);
}

 void Transform::lookAt(glm::vec3 lookTarget)
 {
	 glm::mat4 viewMat = glm::lookAt(m_position, lookTarget, glm::vec3(0, 1, 0));
	 glm::mat4 worldMat = glm::inverse(viewMat);

	 m_rotation = glm::quat_cast(worldMat);
 }

void Transform::rotate(float degrees, glm::vec3 axis )
{
    auto rot = glm::angleAxis( glm::radians(degrees), axis);
    rotate(rot);
}
