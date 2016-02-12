#include "Program.h"

#include "Helpers.h"

#include <assert.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>


bool Program::create(const char* pVertShaderSource, const char* pFragShaderSource)
{
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

void Program::setUniform(const char* uniformName, const glm::mat4& value, int count)
{
	GLuint loc = glGetUniformLocation(m_programID, uniformName);
	assert(loc != -1);
	glUniformMatrix4fv(loc, count, GL_FALSE, glm::value_ptr(value));
}

void Program::setUniform(const char* uniformName, const int value)
{
	GLuint loc = glGetUniformLocation(m_programID, uniformName);
	assert(loc != -1);
	glUniform1i(loc, value);
}
