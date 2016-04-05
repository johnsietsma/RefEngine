#pragma once

#include "RefOpenGL.h"

#include "Program.h"
#include "Mesh.h"
#include "Sampler.h"

#include <bitset>
#include <vector>

struct Renderable
{
    Renderable() = default;
    Renderable(const Program& a_program, const Mesh& a_mesh) :
        Renderable(a_program, a_mesh, std::vector<Sampler>())
        
    {}

    Renderable(const Program& a_program, const Mesh& a_mesh, const std::vector<Sampler> a_samplers) :
        program(a_program),
        mesh(a_mesh),
        samplers(a_samplers)
    {}

    Program program;
    Mesh mesh;
    std::vector<Sampler> samplers;
    GLenum renderMode = GL_FILL;
};
