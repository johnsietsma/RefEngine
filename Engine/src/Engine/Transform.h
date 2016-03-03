#pragma once

#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/quaternion.hpp"


class Transform {
public:
    static const glm::vec3 WORLD_RIGHT;
    static const glm::vec3 WORLD_UP;
    static const glm::vec3 WORLD_FORWARD;

    Transform() :
		Transform( glm::vec3(0), glm::quat(), glm::vec3(1) )
    {}

    Transform(glm::vec3 position);
    Transform(glm::vec3 position, glm::quat rot);
    Transform(glm::vec3 position, glm::quat rot, glm::vec3 scale);

    Transform(glm::vec3 position, glm::vec3 lookPosition);

	glm::vec3 getPosition() const { return m_position;  }
	glm::quat getRotation() const { return m_rotation; }
	glm::vec3 getEulerAngles() const { return glm::eulerAngles(m_rotation); }
	glm::vec3 getScale() const { return m_scale;  }

	glm::vec3 getForward() const { return glm::normalize(m_rotation * glm::vec3(0, 0, 1)); }
	glm::vec3 getRight() const { return glm::normalize(m_rotation * glm::vec3(1, 0, 0)); }
	glm::vec3 getUp() const { return glm::normalize(m_rotation * glm::vec3(0, 1, 0)); }

    glm::mat4 getMatrix() const;
    glm::mat4 getInverseMatrix() const { return glm::inverse(getMatrix()); }

	void setPosition(glm::vec3 position) { m_position = position; }
	void setRotation(glm::quat rotation) { m_rotation = rotation; }
	void setEulerAngles(glm::vec3 rotation) { m_rotation = glm::quat(rotation); }
	void setScale(glm::vec3 scale) { m_scale = scale; }

	void lookAt(glm::vec3 lookTarget);

	void translate(glm::vec3 positionDelta) { m_position += positionDelta; }
    void rotate(glm::quat rotation) { m_rotation = rotation * m_rotation; }
	void rotate(float degrees, glm::vec3 axis=WORLD_UP);
private:
	glm::vec3 m_position;
	glm::quat m_rotation;
	glm::vec3 m_scale;
};

