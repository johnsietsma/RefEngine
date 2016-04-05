#pragma once

#include "graphics/RefOpenGL.h"

#include <fstream>
#include <iostream>


// Read an entire file from disk.
// Returns an empty string on failure.
std::string ReadFile(const char* fileName);

// Register the OpenGL debugging callback.
void TurnOnOpenGLDebugLogging();