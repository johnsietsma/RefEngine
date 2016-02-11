
#define GLM_SWIZZLE
#include "Camera.h"
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

Camera::Camera(float fovY, float aspectRatio, float near, float far)
	: m_speed(10),
	m_up(0,1,0),
	m_transform(1),
	m_view(1)
{
	setPerspective(fovY, aspectRatio, near, far);
}

Camera::~Camera()
{

}

void Camera::setPerspective(float fovY, float aspectRatio, float near, float far)
{
	m_projection = glm::perspective(glm::pi<float>() * 0.25f, 16 / 9.f, 0.1f, 1000.0f);
	m_projectionView = m_projection * m_view;
}

void Camera::setLookAtFrom(const glm::vec3& from, const glm::vec3& to)
{
	m_view = glm::lookAt(from, to, glm::vec3(0, 1, 0));
	m_transform = glm::inverse(m_view);
	m_projectionView = m_projection * m_view;
}

void Camera::update(float deltaTime)
{
	GLFWwindow* window = glfwGetCurrentContext();

	float frameSpeed = glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? deltaTime * m_speed * 2 : deltaTime * m_speed;	

	// translate
	if (glfwGetKey(window,'W') == GLFW_PRESS)
		m_transform[3] -= m_transform[2] * frameSpeed;
	if (glfwGetKey(window,'S') == GLFW_PRESS)
		m_transform[3] += m_transform[2] * frameSpeed;
	if (glfwGetKey(window,'D') == GLFW_PRESS)
		m_transform[3] += m_transform[0] * frameSpeed;
	if (glfwGetKey(window,'A') == GLFW_PRESS)
		m_transform[3] -= m_transform[0] * frameSpeed;
	if (glfwGetKey(window,'Q') == GLFW_PRESS)
		m_transform[3] += m_transform[1] * frameSpeed;
	if (glfwGetKey(window,'E') == GLFW_PRESS)
		m_transform[3] -= m_transform[1] * frameSpeed;
	
	// check for rotation
	static bool sbMouseButtonDown = false;
	if (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
		static double siPrevMouseX = 0;
		static double siPrevMouseY = 0;

		if (sbMouseButtonDown == false)
		{
			sbMouseButtonDown = true;
			glfwGetCursorPos(window,&siPrevMouseX,&siPrevMouseY);
		}

		double mouseX = 0, mouseY = 0;
		glfwGetCursorPos(window,&mouseX,&mouseY);

		double iDeltaX = mouseX - siPrevMouseX;
		double iDeltaY = mouseY - siPrevMouseY;

		siPrevMouseX = mouseX;
		siPrevMouseY = mouseY;

		glm::mat4 mMat;
		
		// pitch
		if (iDeltaY != 0)
		{
			mMat = glm::axisAngleMatrix(m_transform[0].xyz(), (float)-iDeltaY / 150.0f);
			m_transform[0] = mMat * m_transform[0];
			m_transform[1] = mMat * m_transform[1];
			m_transform[2] = mMat * m_transform[2];
		}

		// yaw
		if (iDeltaX != 0)
		{
			mMat = glm::axisAngleMatrix( m_up, (float)-iDeltaX / 150.0f );
			m_transform[0] = mMat * m_transform[0];
			m_transform[1] = mMat * m_transform[1];
			m_transform[2] = mMat * m_transform[2];
		}
	}
	else
	{
		sbMouseButtonDown = false;
	}

	m_view = glm::inverse(m_transform);
	m_projectionView = m_projection * m_view;
}

glm::vec3 Camera::screenPositionToDirection(float x, float y) const {
	
	int width = 0, height = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);

	glm::vec3 screenPos(x / width * 2 - 1, (y / height * 2 - 1) * -1, -1);

	screenPos.x /= m_projection[0][0];
	screenPos.y /= m_projection[1][1];

	return glm::normalize(m_transform * glm::vec4(screenPos, 0)).xyz();
}

glm::vec3 Camera::pickAgainstPlane(float x, float y, const glm::vec4& plane) const {

	int width = 0, height = 0;
	glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);

	glm::vec3 screenPos(x / width * 2 - 1, (y / height * 2 - 1) * -1, -1);

	screenPos.x /= m_projection[0][0];
	screenPos.y /= m_projection[1][1];

	glm::vec3 dir = glm::normalize(m_transform * glm::vec4(screenPos, 0)).xyz();

	float d = (plane.w - glm::dot(m_transform[3].xyz(), plane.xyz()) / glm::dot(dir, plane.xyz()));

	return m_transform[3].xyz() + dir * d;
}