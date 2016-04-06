#include "Helpers.h"

#include <iostream>
#include <vector>




std::string ReadFile(const char* fileName)
{
    std::ifstream ifs(fileName, std::ios::in | std::ios::binary | std::ios::ate);
    if (!ifs.is_open()) return "";

    std::ifstream::pos_type fileSize = ifs.tellg();
    if ((unsigned int)fileSize == 0) return "";

    ifs.seekg(0, std::ios::beg);

    std::vector<char> bytes((unsigned int)fileSize);
    ifs.read(&bytes[0], fileSize);

    return std::string(&bytes[0], (unsigned int)fileSize);
}


void openglCallbackFunction(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) 
    {
        // Severity is not low, medium or high.
        // Skip these messages as they tend not to be helpful and drown out useful messages.
        // glDebugMessageControl doesn't seem to be able to filter these out.
        return;
    }

    std::cout << "---------------------opengl-callback-start------------" << std::endl;
    std::cout << "message: " << message << std::endl;
    std::cout << "type: ";
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        std::cout << "ERROR";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        std::cout << "DEPRECATED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        std::cout << "UNDEFINED_BEHAVIOR";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        std::cout << "PORTABILITY";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        std::cout << "PERFORMANCE";
        break;
    case GL_DEBUG_TYPE_OTHER:
        std::cout << "OTHER";
        break;
    }
    std::cout << std::endl;

    std::cout << "id: " << id;
    std::cout << "severity: ";
    switch (severity) {
    case GL_DEBUG_SEVERITY_LOW:
        std::cout << "LOW";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        std::cout << "MEDIUM";
        break;
    case GL_DEBUG_SEVERITY_HIGH:
        std::cout << "HIGH";
        break;
    default:
        std::cout << "Unkown severity: " << severity << std::endl;
    }
    std::cout << std::endl;
    std::cout << "---------------------opengl-callback-end--------------" << std::endl;
}

void TurnOnOpenGLDebugLogging()
{
    if (glDebugMessageCallback == nullptr) return;
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(openglCallbackFunction, nullptr);
    GLuint unusedIds = 0;
    glDebugMessageControl(
        GL_DONT_CARE, // source
        GL_DONT_CARE, // type
        GL_DONT_CARE, // severity
        0,
        &unusedIds,
        true);
}