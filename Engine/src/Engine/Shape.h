#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <vector>


class Shape
{
public:
    virtual bool isBehind(glm::vec4 plane) = 0;
    virtual void expand(glm::vec3 position) = 0;
};

class AABB : public Shape
{
public:
    AABB() {}
    ~AABB() {}

    bool isBehind(glm::vec4 plane) override
    {
        return true;
    }

    void expand(glm::vec3 position) override
    {

    }

    glm::vec3 min, max;
};

class Sphere : public Shape
{
public:
    Sphere() : centre(0), radius(0) { }
    ~Sphere() {}

    bool isBehind(glm::vec4 plane) override
    {
        float d = glm::dot(glm::vec3(plane), centre) + plane.w;
        return d + radius < 0;
    }

    void expand(glm::vec3 position) override
    {
        float pointDistance = glm::length(position);
        radius = std::fmax(pointDistance, radius);
    }

    glm::vec3 centre;
    float radius;
};
