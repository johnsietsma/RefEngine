#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class Camera {
public:

	Camera(float fovY, float aspectRatio, float near, float far);
	virtual ~Camera();

	void	update(float deltaTime);

	void	setSpeed(float speed)	{ m_speed = speed; }

	void	setPerspective(float fovY, float aspectRatio, float near, float far);

	void	setLookAtFrom(const glm::vec3& from, const glm::vec3& to);

	const glm::mat4&	getTransform() const		{ return m_transform; }
	const glm::mat4&	getProjection() const		{ return m_projection; }
	const glm::mat4&	getView() const				{ return m_view; }
	const glm::mat4&	getProjectionView() const	{ return m_projectionView; }

	// returns a world-space normalized vector pointing away from the camera's world-space position
	glm::vec3			screenPositionToDirection(float x, float y) const;

	// returns the point of intersection of a camera ray and a world-space plane
	// the plane has a normal of XYZ and is offset from (0,0,0) by -W in the direction of the normal
	glm::vec3			pickAgainstPlane(float x, float y, const glm::vec4& plane) const;

private:
	
	float		m_speed;
	glm::vec3	m_up;
	glm::mat4	m_transform;
	glm::mat4	m_projection;
	glm::mat4	m_view;
	glm::mat4	m_projectionView;
};