#pragma once

#include "gl_core_4_4.h"

#include "Program.h"
#include "Mesh.h"
#include "Sampler.h"

#include <vector>

struct Renderable
{
    GLenum renderMode = GL_FILL;
    Program program;
    Mesh mesh;
    std::vector<Sampler> samplers;
};
