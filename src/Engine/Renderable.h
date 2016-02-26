#pragma once

#include "Program.h"
#include "Mesh.h"
#include "Sampler.h"

#include <vector>

struct Renderable
{
    Program program;
    Mesh mesh;
    std::vector<Sampler> samplers;
};
