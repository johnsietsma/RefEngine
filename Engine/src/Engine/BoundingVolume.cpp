#include "BoundingVolume.h"

#include "Shape.h"

#include <glm/geometric.hpp>

bool BoundingVolume::isInsideFrustum(glm::vec3 center, glm::vec4* pPlanes)
{
    // If no shapes have been added always be visible
    if (m_shapes.size() == 0) return true;

    int insideShapeCount = 0;
    for (auto pShape : m_shapes)
    {
        bool isOutside = false;
        for (int i = 0; i < 6; i++) {
            if (pShape->isBehind(pPlanes[i]))
            {
                // This shape is not completely inside
                isOutside = true;
                break;
            }
        }

        // It's inside the frustum
        if( !isOutside ) insideShapeCount++;
    }

    // Return true if any of the shapes where inside the frustum
    return insideShapeCount > 0;
}

void BoundingVolume::addBoundingSphere(glm::vec3 center, std::vector<glm::vec3> pointsToFit)
{
    auto pShape = std::make_shared<Sphere>();
    for( glm::vec3& point : pointsToFit )
    {
        pShape->expand(point);
    }

    m_shapes.push_back(pShape);
}


void BoundingVolume::addBoundingSphere(glm::vec3 center, std::vector<Vertex_FBX> pointsToFit)
{
    auto pShape = std::make_shared<Sphere>();
    for (auto& vertex : pointsToFit)
    {
        pShape->expand( glm::vec3(vertex.position) );
    }

    m_shapes.push_back(pShape);
}
