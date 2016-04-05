#pragma once

#include "RefOpenGL.h"

#include <glm/mat4x4.hpp>
#include <unordered_map>

/*
    A Program contains shaders linked together to run as part of the render pipeline.
*/

class Program
{
public:
    Program() : m_programID((GLuint)-1) {};
    ~Program() = default;

    bool isValid() const { return m_programID != (GLuint)-1; }
    GLuint getId() const { return m_programID; }
    void setId(GLuint id) { m_programID = id; }

    bool create(const char* pVertShaderSource, const char* pFragShaderSource);
    void destroy();

    bool hasUniform(const char* pUniformName);

    void setUniform(const char* uniformName, const int value);
    void setUniform(const char* uniformName, const unsigned int value);
    void setUniform(const char* uniformName, const float value);
    void setUniform(const char* uniformName, const glm::vec3& value, int count=1);
    void setUniform(const char* uniformName, const glm::vec4& value, int count=1);
    void setUniform(const char* uniformName, const glm::mat4& value, int count=1);

private:
    GLuint getUniformLocation(const char* uniformName);
    GLuint getUniformLocationChecked(const char* uniformName);
    GLuint m_programID;

    std::unordered_map<size_t, GLuint> m_uniformLocationCache;
};

