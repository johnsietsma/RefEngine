#include "ParticleEmitterGameObject.h"

#include "engine/ResourceCreator.h"

#include "gameObjects/CameraGameObject.h"
#include "gameObjects/Particle.h"

#include <assert.h>


ParticleEmitterGameObject::ParticleEmitterGameObject(const ParticleEmitterConfig& config, const std::weak_ptr<CameraGameObject> pBillboardCamera) :
    m_config(config),
    m_pBillboardCamera(pBillboardCamera),
    m_pParticles(nullptr),
    m_pVertices(nullptr),
    m_firstDeadIndex(-1),
    m_emitTimer(-1)
{

}

ParticleEmitterGameObject::~ParticleEmitterGameObject()
{

}

bool ParticleEmitterGameObject::create()
{
    assert(!isValid());

    Renderable renderable;

    renderable.program = ResourceCreator::createProgram("color", "vertexColor");
    if (!renderable.program.isValid()) return false;

    m_emitTimer = 0;

    // The particle array will be arranged so the alive particles are at the start
    //   and the dead particles are at the end.
    m_pParticles = new Particle[m_config.particleCount];

    // There are no alive particles, so the first dead particle is at index 0.
    m_firstDeadIndex = 0;

    // Each particle is a quad, so has 4 verts.
    unsigned int vertCount = m_config.particleCount * 4;

    // This is the buffer that we will send to OpenGL.
    m_pVertices = new Vertex_PositionColor[vertCount];

    // A quad needs 6 indices to index its 2 tris. Calculate how many indexes we'll need for all our particles.
    unsigned int indexCount = m_config.particleCount * 6;
    unsigned int* pIndexData = new unsigned int[indexCount];

    // Setup all the indices. Each index points to one vert.
    for (unsigned int i = 0; i < m_config.particleCount; i++)
    {
        unsigned int indexStart = i * 6; // The index of the first
        unsigned int vertStart = i * 4; // The first index into the buffer that contain the triangle indexes.

        // Tri 1
        pIndexData[indexStart + 0] = vertStart + 0;
        pIndexData[indexStart + 1] = vertStart + 1;
        pIndexData[indexStart + 2] = vertStart + 2;

        // Tri 2
        pIndexData[indexStart + 3] = vertStart + 0;
        pIndexData[indexStart + 4] = vertStart + 2;
        pIndexData[indexStart + 5] = vertStart + 3;
    }

    Primitive vertexPrimitive = Primitive::create( m_pVertices, vertCount );
    Buffer indexBuffer(pIndexData, indexCount);
    renderable.mesh.create( vertexPrimitive, indexBuffer );

    m_renderables.push_back(renderable);

    // We've given the indcies to OpenGL, it's safe to delete them now.
    delete[] pIndexData;

    return true;
}


void ParticleEmitterGameObject::destroy()
{
    if (!isValid()) return;

    GameObject::destroy();

    delete[] m_pParticles;
    delete[] m_pVertices;
    m_firstDeadIndex = -1;
}

void ParticleEmitterGameObject::emit()
{
    assert(isValid());

    // Check that there is room for more particles
    if (m_firstDeadIndex >= m_config.particleCount) return;

    // Grab a dead particle and increase our particle count
    Particle& particle = m_pParticles[m_firstDeadIndex++];

    // Place the particle at the emitter's position
    particle.position = m_position;

    particle.lifetime = 0;

    // Get a random time we should live for
    particle.lifespan =
        (rand() / (float)RAND_MAX) // Calculate a random value between 0 and 1
        * (m_config.lifespanMax - m_config.lifespanMin) // Scale the random value to our possible lifespace range
        + m_config.lifespanMin; // Shift the lifetime so it's past the minimum lifespan

    particle.color = m_config.startColor;
    particle.size = m_config.startSize;

    // Get a random speed we should travel at.
    // Speed is a scalar, it has no direction
    float speed = (rand() / (float)RAND_MAX) *
        (m_config.velocityMax - m_config.velocityMin)
        + m_config.velocityMin;

    // Calculate a random direction for the particle.
    particle.velocity.x = (rand() / (float)RAND_MAX) * 2 - 1;
    particle.velocity.y = (rand() / (float)RAND_MAX) * 2 - 1;
    particle.velocity.z = (rand() / (float)RAND_MAX) * 2 - 1;

    // Scale the velocity vector (its direction) by the speed (how fast we should go in that direction)
    particle.velocity = glm::normalize(particle.velocity) * speed;

}

void ParticleEmitterGameObject::update(float deltaTime)
{
    m_emitTimer += deltaTime;

    // Convert from particles per second to seconds per particle.
    // In orther words, how lond should we wait before we emit a particle?
    float timePerParticle = 1.f / m_config.emitRate;

    while( m_emitTimer > timePerParticle)
    {
        emit();
        m_emitTimer -= timePerParticle;
    }

    if (static_cast<int>(m_firstDeadIndex) < 0) return;

    std::shared_ptr<CameraGameObject> pBillboardCamera = m_pBillboardCamera.lock();
    if (pBillboardCamera == nullptr) return;
    const Transform& camTransform = pBillboardCamera->getTransform();


    for (unsigned int particleIndex = 0; particleIndex < m_firstDeadIndex; particleIndex++)
    {
        // Grab our particle
        Particle* particle = &m_pParticles[particleIndex];

        // Keep track of how long we've been alive for
        particle->lifetime += deltaTime;

        // Should we be dead?
        if (particle->lifetime > particle->lifespan)
        {
            // We're dead, take an alive particle from the end of the list
            //   and put it in the dead particle's position. This keeps all the
            //   alive particles together without any gaps.
            *particle = m_pParticles[m_firstDeadIndex - 1];

            // Now we can just reduce the dead index to finsish removing the particle
            m_firstDeadIndex--;
            continue;
        }

        // Update the particles position
        particle->position += particle->velocity * deltaTime;

        // Normalize the time the particles has been alive for.
        float timeNorm = particle->lifetime / particle->lifespan;

        // Lerp from the start to end values using our normalized time.
        particle->size = glm::mix(m_config.startSize, m_config.endSize, timeNorm);
        particle->color = glm::mix(m_config.startColor, m_config.endColor, timeNorm);

        // Each particle has 4 verts. Figure out the index of the first vertex in this quad.
        unsigned int startVertexIndex = particleIndex * 4;

        // Position a quad at the origin
        float halfSize = particle->size * 0.5f;
        m_pVertices[startVertexIndex + 0].position = glm::vec4( halfSize,  halfSize, 0, 1);
        m_pVertices[startVertexIndex + 1].position = glm::vec4(-halfSize,  halfSize, 0, 1);
        m_pVertices[startVertexIndex + 2].position = glm::vec4(-halfSize, -halfSize, 0, 1);
        m_pVertices[startVertexIndex + 3].position = glm::vec4( halfSize, -halfSize, 0, 1);

        // Just simple flat color for now
        m_pVertices[startVertexIndex + 0].color = particle->color;
        m_pVertices[startVertexIndex + 1].color = particle->color;
        m_pVertices[startVertexIndex + 2].color = particle->color;
        m_pVertices[startVertexIndex + 3].color = particle->color;

        // Create a billboard matrix that will transform verts to face towards the camera

        // z axis is the normalized vector that points towards the camera
        glm::vec3 zAxis = glm::normalize(glm::vec3(camTransform.getPosition()) - particle->position);

        // Calculate the y axis using the cameras up axis and the particles new forward vector
        glm::vec3 xAxis = glm::cross(camTransform.getUp(), zAxis);

        // Finally, find our y axis using the forward and left vectors
        glm::vec3 yAxis = glm::cross(zAxis, xAxis);

        // Create a transformation matrix from the axes.
        glm::mat4 billboard(
            glm::vec4(xAxis, 0),
            glm::vec4(yAxis, 0),
            glm::vec4(zAxis, 0),
            glm::vec4(0,0,0, 1)
            );

        // Position each of the the 4 billboard particles
        positionBillboardParticle(startVertexIndex + 0, billboard, particle);
        positionBillboardParticle(startVertexIndex + 1, billboard, particle);
        positionBillboardParticle(startVertexIndex + 2, billboard, particle);
        positionBillboardParticle(startVertexIndex + 3, billboard, particle);
    }
}

void ParticleEmitterGameObject::positionBillboardParticle(unsigned int vertexIndex, const glm::mat4& billboardMat, const Particle* particle)
{
    m_pVertices[vertexIndex].position =
        billboardMat * m_pVertices[vertexIndex].position  // Transform the vert position so that the qaud will be facing the camera
        + glm::vec4(particle->position, 0); // Move the vert to the particles position
}

void ParticleEmitterGameObject::preDraw(const CameraGameObject& camera, const Light* pLight)
{
    assert(m_renderables.size() > 0);

    // There is only one renderable
    Renderable& renderable = m_renderables[0];

    glBindBuffer(GL_ARRAY_BUFFER, renderable.mesh.getVBOs()[0]);

    // We've moved the particles, so we'll upload the new particle vertex data.
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_firstDeadIndex * 4 * sizeof(Vertex_PositionColor), m_pVertices);

    // Make sure we only draw the alive particles
    renderable.mesh.setIndexCount(m_firstDeadIndex * 6);
}
