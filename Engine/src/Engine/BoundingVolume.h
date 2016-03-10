#pragma once

#include "Vertex.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <memory>
#include <vector>

class Shape;


class BoundingVolume
{
public:
    bool isInsideFrustum(glm::vec3 center, glm::vec4* pPlanes);

    void addBoundingSphere(glm::vec3 center, std::vector<glm::vec3> pointsToFit);

    //TODO: Remove the FBX dep
    void addBoundingSphere(glm::vec3 center, std::vector<Vertex_FBX> pointsToFit );

private:
    std::vector<std::shared_ptr<Shape>> m_shapes;
};