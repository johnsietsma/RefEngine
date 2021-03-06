#pragma once

#include "engine/GameObject.h"
#include "graphics/Mesh.h"
#include "graphics/Program.h"

class CameraGameObject;
struct Particle;

struct ParticleEmitterConfig
{
    ParticleEmitterConfig() : particleCount(-1) {}

    unsigned int particleCount;

    float emitRate; // How many particles per second should we emit.

    float lifespanMin;
    float lifespanMax;

    float velocityMin;
    float velocityMax;

    float startSize; // In world units
    float endSize; // How big the particle will be just before it dies

    glm::vec4 startColor;
    glm::vec4 endColor;
};

/*
    A simple particle emitter that emits in random direcions from a single point.
    Particles are update on the CPU.
*/
class ParticleEmitterGameObject : public GameObject
{
public:
    ParticleEmitterGameObject(const ParticleEmitterConfig& config, const std::weak_ptr<CameraGameObject> pBillboardCamera);
    ~ParticleEmitterGameObject();

    bool isValid() { return m_firstDeadIndex != -1; }

    bool create() override;
    void destroy() override;
    void update(float deltaTime) override;
    void preDraw(const CameraGameObject& camera, const Light* pLight) override;

    ParticleEmitterGameObject(const ParticleEmitterGameObject& rhs) = delete;
    void operator =(const ParticleEmitterGameObject& rhs) = delete;

private:
    void emit();
    void positionBillboardParticle(unsigned int vertexIndex, const glm::mat4& billboardMat, const Particle* particle);

    const ParticleEmitterConfig m_config;
    const std::weak_ptr<CameraGameObject> m_pBillboardCamera;

    glm::vec3 m_position;
    Particle* m_pParticles;
    Vertex_PositionColor* m_pVertices;

    unsigned int m_firstDeadIndex;
    float m_emitTimer;
};
