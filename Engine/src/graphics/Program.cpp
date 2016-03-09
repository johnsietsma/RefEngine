#include "graphics/Program.h"

#include <assert.h>
#include <glm/gtc/type_ptr.hpp>

#include <functional>
#include <iostream>
#include <string>


// Check whether a shader compile succeeded.
// Logs to stderr and returns false if ti has failed.
bool CheckCompileStatus(GLuint shaderId)
{
    GLint result = GL_FALSE;
    int logLength = 0;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
    if (result != GL_TRUE) {
        char* logBuffer = NULL;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
        logBuffer = new char[logLength];
        glGetShaderInfoLog(shaderId, logLength, NULL, logBuffer);
        std::cerr << "Compile Error: " << logBuffer << std::endl;
        delete[] logBuffer;
        return false;
    }
    return true;
}

// Check whether a program link succeeded.
// Logs to stderr and returns false if ti has failed.
bool CheckLinkStatus(GLuint programId)
{
    GLint result = GL_FALSE;
    int logLength = 0;
    glGetProgramiv(programId, GL_LINK_STATUS, &result);
    if (result != GL_TRUE) {
        char* logBuffer = NULL;
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logLength);
        logBuffer = new char[logLength];
        glGetProgramInfoLog(programId, logLength, NULL, logBuffer);
        std::cerr << "Link Error: " << logBuffer << std::endl;
        delete[] logBuffer;
        return false;
    }
    return true;
}


bool Program::create(const char* pVertShaderSource, const char* pFragShaderSource)
{
    assert(!isValid());
    assert(pVertShaderSource != nullptr);
    assert(pFragShaderSource != nullptr);

    GLuint vertShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShaderId, 1, &pVertShaderSource, 0);
    glCompileShader(vertShaderId);
    if (!CheckCompileStatus(vertShaderId))
        return false;

    GLuint fragShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShaderId, 1, &pFragShaderSource, 0);
    glCompileShader(fragShaderId);
    if (!CheckCompileStatus(fragShaderId))
        return false;

    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertShaderId);
    glAttachShader(programId, fragShaderId);
    glLinkProgram(programId);
    if (!CheckLinkStatus(programId))
        return false;

    // Sucessfully created the program, it's safe to store it now
    m_programID = programId;

    glDeleteShader(vertShaderId);
    glDeleteShader(fragShaderId);

    return true;
}

void Program::destroy()
{
    if (m_programID == (GLuint)-1) return;

    glDeleteProgram(m_programID);
    m_programID = (GLuint)-1;
}

bool Program::hasUniform(const char* pUniformName)
{
    return getUniformLocation(pUniformName) != -1;
}

void Program::setUniform(const char* uniformName, const int value)
{
    GLuint loc = getUniformLocation(uniformName);
    if (loc != -1) glUniform1i(loc, value);
}

void Program::setUniform(const char* uniformName, const unsigned int value)
{
    GLuint loc = getUniformLocation(uniformName);
    if (loc != -1) glUniform1ui(loc, value);
}

void Program::setUniform(const char* uniformName, const float value)
{
    GLuint loc = getUniformLocation(uniformName);
    if (loc != -1) glUniform1f(loc, value);
}

void Program::setUniform(const char* uniformName, const glm::vec3& value, int count)
{
    GLuint loc = getUniformLocation(uniformName);
    if( loc!=-1 )  glUniform3fv(loc, count, glm::value_ptr(value));
}

void Program::setUniform(const char* uniformName, const glm::vec4& value, int count)
{
    GLuint loc = getUniformLocation(uniformName);
    if( loc!=-1 )  glUniform2fv(loc, count, glm::value_ptr(value));
}

void Program::setUniform(const char* uniformName, const glm::mat4& value, int count)
{
    GLuint loc = getUniformLocation(uniformName);
    if( loc!=-1 )  glUniformMatrix4fv(loc, count, GL_FALSE, glm::value_ptr(value));
}


GLuint Program::getUniformLocation(const char* uniformName)
{
    size_t nameHash = std::hash<std::string>{} ( std::string(uniformName) );

    if (m_uniformLocationCache.find(nameHash) != m_uniformLocationCache.end())
    {
        return m_uniformLocationCache[nameHash];
    }

    // Cache the result for efficiency reasons
    GLuint loc = glGetUniformLocation(m_programID, uniformName);
    m_uniformLocationCache[nameHash] = loc;
    return loc;
}


GLuint Program::getUniformLocationChecked(const char* uniformName)
{
    GLuint loc = getUniformLocation(uniformName);
    if (loc == -1) {
        std::cerr << "Couldn't find uniform: " << uniformName << std::endl;
    }
    return loc;
}
