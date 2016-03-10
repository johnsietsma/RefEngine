
#include "Engine/Camera.h"
#include <GLFW/glfw3.h>
#include <glm/gtx/quaternion.hpp>

using glm::vec3;
using glm::vec4;

Camera::Camera(const Transform& transform, float fovY, float aspectRatio, float near, float far) :
    m_transform(transform),
    m_speed(10)
{
    setPerspective(fovY, aspectRatio, near, far);
}

Camera::~Camera()
{

}


void Camera::getFrustumPlanes( glm::vec4 (&pPlanes)[6] ) const
{
    glm::mat4 transform = getProjectionViewTransform();

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


void Camera::update(float deltaTime)
{
    GLFWwindow* window = glfwGetCurrentContext();

    float frameSpeed = glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? deltaTime * m_speed * 2 : deltaTime * m_speed;

    // translate
    glm::vec3 moveDir;
    if (glfwGetKey(window,'W') == GLFW_PRESS)
        moveDir = -m_transform.getForward();
    if (glfwGetKey(window,'S') == GLFW_PRESS)
        moveDir = m_transform.getForward();
    if (glfwGetKey(window,'D') == GLFW_PRESS)
        moveDir = m_transform.getRight();
    if (glfwGetKey(window,'A') == GLFW_PRESS)
        moveDir = -m_transform.getRight();
    if (glfwGetKey(window,'Q') == GLFW_PRESS)
        moveDir = m_transform.getUp();
    if (glfwGetKey(window,'E') == GLFW_PRESS)
        moveDir = -m_transform.getUp();

    m_transform.translate(moveDir * frameSpeed);

    // check for rotation
    static bool sbMouseButtonDown = false;
    if (glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_2) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_SPACE))
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
            auto rotation = glm::angleAxis((float)-iDeltaY / 150.0f, m_transform.getRight());
            m_transform.rotate( rotation );
        }

        // yaw
        if (iDeltaX != 0)
        {
            auto rotation = glm::angleAxis((float)-iDeltaX / 150.0f, glm::vec3(0,1,0)   );
            m_transform.rotate( rotation );
        }
    }
    else
    {
        sbMouseButtonDown = false;
    }
}

glm::vec3 Camera::screenPositionToDirection(float x, float y) const {

    int width = 0, height = 0;
    glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);

    glm::vec3 screenPos(x / width * 2 - 1, (y / height * 2 - 1) * -1, -1);

    screenPos.x /= m_projection[0][0];
    screenPos.y /= m_projection[1][1];

    const auto& transform = m_transform.getMatrix();
    return glm::normalize(transform * glm::vec4(screenPos, 0)).xyz();
}

glm::vec3 Camera::pickAgainstPlane(float x, float y, const glm::vec4& plane) const {

    int width = 0, height = 0;
    glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);

    glm::vec3 screenPos(x / width * 2 - 1, (y / height * 2 - 1) * -1, -1);

    screenPos.x /= m_projection[0][0];
    screenPos.y /= m_projection[1][1];

    glm::vec3 dir = glm::normalize(m_transform.getMatrix() * glm::vec4(screenPos, 0)).xyz();

    float d = (plane.w - glm::dot(m_transform.getPosition(), plane.xyz()) / glm::dot(dir, plane.xyz()));

    return m_transform.getPosition().xyz() + dir * d;
}
