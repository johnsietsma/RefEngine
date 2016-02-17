#pragma once

#include "GameObject.h"
#include "Mesh.h"
#include "Program.h"

class Camera;
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
class ParticleEmitter : public GameObject
{
public:
	ParticleEmitter(const ParticleEmitterConfig& config, const Camera* pBillboardCamera);
	~ParticleEmitter();

	bool isValid() { return m_firstDeadIndex != -1; }

	bool create() override;
	void destroy() override;
	void update(float deltaTime) override;
	void draw(const Camera& camera) override;

	ParticleEmitter(const ParticleEmitter& rhs) = delete;
	void operator =(const ParticleEmitter& rhs) = delete;

private:
	void emit();
	void billboardParticle(unsigned int vertexIndex, const glm::mat4& billboardMat, const Particle* particle);

    const ParticleEmitterConfig m_config;
    const Camera* m_pBillboardCamera;

    glm::vec3 m_position;
	Particle* m_pParticles;
	Vertex_PositionColor* m_pVertices;

	unsigned int m_firstDeadIndex;
	float m_emitTimer;

	Mesh m_mesh;
	Program m_program;
};
