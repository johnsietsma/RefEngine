#pragma once

#include "gl_core_4_4.h"

#include <glm/mat4x4.hpp>

/*
	A Program contains shaders linked together to run as part of the render pipeline.
*/

class Program
{
public:
	Program() : m_programID((GLuint)-1) {};
	~Program();

	bool isValid() { return m_programID != (GLuint)-1; }
	GLuint getId() const { return m_programID;  }

	bool create(const char* pVertShaderSource, const char* pFragShaderSource);
	void destroy();

	void setUniform(const char* uniformName, const glm::mat4& value);
	void setUniform(const char* uniformName, const int value);

private:
	GLuint m_programID;
};

