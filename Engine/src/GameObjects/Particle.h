#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

/*
	Represents particles properties.
	This is keep on the CPU side and is used to control
	the particles sent to the GPU.
*/
struct Particle
{
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec4 color;

	float size; // In world units
	float lifetime; // How long we've been alive for (sceonds)
	float lifespan; // How long we should live for
};
