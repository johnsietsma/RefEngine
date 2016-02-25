#pragma once

#include "gl_core_4_4.h"

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

    bool isValid() { return m_programID != (GLuint)-1; }
    GLuint getId() const { return m_programID;  }

    bool create(const char* pVertShaderSource, const char* pFragShaderSource);
    void destroy();

    bool hasUniform(const char* pUniformName);

    void setUniform(const char* uniformName, const glm::mat4& value, int count=1);
    void setUniform(const char* uniformName, const int value);

private:
    GLuint getUniformLocation(const char* uniformName);
    GLuint getUniformLocationChecked(const char* uniformName);
    GLuint m_programID;

    std::unordered_map<size_t, GLuint> m_uniformLocationCache;
};

