#include "FBXNode.h"

#include <glm/matrix.hpp>

void FBXNode::updateGlobalTransform()
{
    if (m_parent != nullptr)
        m_globalTransform = m_parent->m_globalTransform * m_localTransform;
    else
        m_globalTransform = m_localTransform;

    for (auto child : m_children)
        child->updateGlobalTransform();
}

void FBXCameraNode::updateGlobalTransform()
{
    if (m_parent != nullptr)
        m_globalTransform = m_parent->m_globalTransform * m_localTransform;
    else
        m_globalTransform = m_localTransform;

    m_viewMatrix = glm::inverse( m_globalTransform );

    for (auto child : m_children)
        child->updateGlobalTransform();
}
