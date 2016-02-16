#pragma once

#include "Mesh.h"
#include "Program.h"

struct Particle;

struct ParticleEmitterConfig
{
	ParticleEmitterConfig() : particleCount(-1) {}

	unsigned int particleCount;

	float emitRate;

	float lifespanMin;
	float lifespanMax;

	float velocityMin;
	float velocityMax;

	float startSize;
	float endSize;

	glm::vec4 startColor;
	glm::vec4 endColor;
};

/*
	
*/
class ParticleEmitter
{
public:
	ParticleEmitter();
	~ParticleEmitter();

	bool isValid() { return m_config.particleCount != -1; }
	bool create( ParticleEmitterConfig config );
	void destroy();

	void update(float deltaTime, const glm::mat4& camMatrix);
	void draw(const glm::mat4& projView);

	ParticleEmitter(const ParticleEmitter& rhs) = delete;
	void operator =(const ParticleEmitter& rhs) = delete;

private:
	void emit();
	void billboardParticle(unsigned int vertexIndex, const glm::mat4& billboardMat, const Particle* particle);

	glm::vec3 m_position;

	Particle* m_pParticles;
	Vertex_PositionColor* m_pVertices;

	unsigned int m_firstDeadIndex;

	ParticleEmitterConfig m_config;
	float m_emitTimer;

	Mesh m_mesh;
	Program m_program;
};
