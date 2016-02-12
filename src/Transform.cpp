#include "Transform.h"

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtx/transform.hpp"


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

 glm::mat4 Transform::GetMatrix() const
{ 
	return glm::translate(m_position) * glm::mat4_cast(m_rotation) * glm::scale(m_scale);
}

 void Transform::LookAt(glm::vec3 lookTarget)
 {
	 glm::mat4 viewMat = glm::lookAt(m_position, lookTarget, glm::vec3(0, 1, 0));
	 glm::mat4 worldMat = glm::inverse(viewMat);

	 m_rotation = glm::quat_cast(worldMat);
 }

