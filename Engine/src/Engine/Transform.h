#pragma once

#include "glm/vec3.hpp"
#include "glm/gtc/quaternion.hpp"


class Transform {
public:
    Transform() :
		Transform( glm::vec3(0), glm::quat(), glm::vec3(1) )
    {}

    Transform(glm::vec3 position);
    Transform(glm::vec3 position, glm::quat rot);
    Transform(glm::vec3 position, glm::quat rot, glm::vec3 scale);

	glm::vec3 GetPosition() const { return m_position;  }
	glm::quat GetRotation() const { return m_rotation; }
	glm::vec3 GetEulerAngles() const { return glm::eulerAngles(m_rotation); }
	glm::vec3 GetScale() const { return m_scale;  }

	glm::vec3 GetForward() const { return glm::normalize(m_rotation * glm::vec3(0, 0, 1)); }
	glm::vec3 GetRight() const { return glm::normalize(m_rotation * glm::vec3(1, 0, 0)); }
	glm::vec3 GetUp() const { return glm::normalize(m_rotation * glm::vec3(0, 1, 0)); }

	glm::mat4 GetMatrix() const;

	void SetPosition(glm::vec3 position) { m_position = position; }
	void SetRotation(glm::quat rotation) { m_rotation = rotation; }
	void SetEulerAngles(glm::vec3 rotation) { m_rotation = glm::quat(rotation); }
	void SetScale(glm::vec3 scale) { m_scale = scale; }

	void LookAt(glm::vec3 lookTarget);

	void Translate(glm::vec3 positionDelta) { m_position += positionDelta; }
	void Rotate(glm::quat rotation) { m_rotation = rotation * m_rotation; }

private:
	glm::vec3 m_position;
	glm::quat m_rotation;
	glm::vec3 m_scale;
};

