#include "ParticleEmitter.h"

#include "Camera.h"
#include "Particle.h"
#include "ResourceCreator.h"

#include <assert.h>


ParticleEmitter::ParticleEmitter(const ParticleEmitterConfig& config, const Camera* pBillboardCamera) :
    m_pBillboardCamera(pBillboardCamera),
    m_config(config),
	m_pParticles(nullptr),
	m_pVertices(nullptr),
	m_firstDeadIndex(-1),
    m_emitTimer(-1)
{

}

ParticleEmitter::~ParticleEmitter()
{

}

bool ParticleEmitter::create()
{
	assert(!isValid());

	m_program = ResourceCreator::CreateProgram("./data/shaders/color.vert", "./data/shaders/vertexColor.frag");
	if (!m_program.isValid()) return false;

	m_emitTimer = 0;

	m_pParticles = new Particle[m_config.particleCount];
	m_firstDeadIndex = 0;

	unsigned int vertCount = m_config.particleCount * 4;
	m_pVertices = new Vertex_PositionColor[vertCount];


	unsigned int indexCount = m_config.particleCount * 6;
	unsigned int* pIndexData = new unsigned int[indexCount];

	for (unsigned int i = 0; i < m_config.particleCount; i++)
	{
		unsigned int indexStart = i * 6;
		unsigned int vertStart = i * 4;

		pIndexData[indexStart + 0] = vertStart + 0;
		pIndexData[indexStart + 1] = vertStart + 1;
		pIndexData[indexStart + 2] = vertStart + 2;

		pIndexData[indexStart + 3] = vertStart + 0;
		pIndexData[indexStart + 4] = vertStart + 2;
		pIndexData[indexStart + 5] = vertStart + 3;
	}

	m_mesh.create(m_pVertices, vertCount, pIndexData, indexCount);

	delete pIndexData;

	return true;
}


void ParticleEmitter::destroy()
{
	assert(isValid());

	delete[] m_pParticles;
	delete[] m_pVertices;

	m_mesh.destroy();
}

void ParticleEmitter::emit()
{
	assert(isValid());

	// Check that there is room for more particles
	if (m_firstDeadIndex >= m_config.particleCount) return;

	Particle& particle = m_pParticles[m_firstDeadIndex++];

	particle.position = m_position;

	particle.lifetime = 0;
	particle.lifespan = (rand() / (float)RAND_MAX) *
		(m_config.lifespanMax - m_config.lifespanMin) 
		+ m_config.lifespanMin;

	particle.color = m_config.startColor;
	particle.size = m_config.startSize;

	float speed = (rand() / (float)RAND_MAX) *
		(m_config.velocityMax - m_config.velocityMin) 
		+ m_config.velocityMin;

	particle.velocity.x = (rand() / (float)RAND_MAX) * 2 - 1;
	particle.velocity.y = (rand() / (float)RAND_MAX) * 2 - 1;
	particle.velocity.z = (rand() / (float)RAND_MAX) * 2 - 1;

	particle.velocity = glm::normalize(particle.velocity) * speed;

}

void ParticleEmitter::update(float deltaTime)
{
	m_emitTimer += deltaTime;

	float invEmitRate = 1.f / m_config.emitRate;
	while( m_emitTimer > invEmitRate )
	{
		emit();
		m_emitTimer -= invEmitRate;
	}


	int quadIndex = 0;
	for (unsigned int i = 0; i < m_firstDeadIndex; i++)
	{
		Particle* particle = &m_pParticles[i];

		particle->lifetime += deltaTime;

		if (particle->lifetime > particle->lifespan)
		{
			*particle = m_pParticles[m_firstDeadIndex - 1];
			m_firstDeadIndex--;
			continue;
		}

		particle->position += particle->velocity * deltaTime;

		float timeNorm = particle->lifetime / particle->lifespan;
		particle->size = glm::mix(m_config.startSize, m_config.endSize, timeNorm);
		particle->color = glm::mix(m_config.startColor, m_config.endColor, timeNorm);

		float halfSize = particle->size * 0.5f;

		unsigned int vertexIndex = quadIndex * 4;
		m_pVertices[vertexIndex + 0].position = glm::vec4( halfSize,  halfSize, 0, 1);
		m_pVertices[vertexIndex + 1].position = glm::vec4(-halfSize,  halfSize, 0, 1);
		m_pVertices[vertexIndex + 2].position = glm::vec4(-halfSize, -halfSize, 0, 1);
		m_pVertices[vertexIndex + 3].position = glm::vec4( halfSize, -halfSize, 0, 1);

		m_pVertices[vertexIndex + 0].color = particle->color;
		m_pVertices[vertexIndex + 1].color = particle->color;
		m_pVertices[vertexIndex + 2].color = particle->color;
		m_pVertices[vertexIndex + 3].color = particle->color;

		// create billboard
        const glm::mat4 camMatrix = m_pBillboardCamera->getTransform();
		glm::vec3 zAxis = glm::normalize(glm::vec3(camMatrix[3]) - particle->position);
		glm::vec3 xAxis = glm::cross(glm::vec3(camMatrix[1]), zAxis);
		glm::vec3 yAxis = glm::cross(zAxis, xAxis);

		glm::mat4 billboard(
			glm::vec4(xAxis, 0),
			glm::vec4(yAxis, 0),
			glm::vec4(zAxis, 0),
			glm::vec4(0,0,0, 1)
			);

		billboardParticle(vertexIndex + 0, billboard, particle);
		billboardParticle(vertexIndex + 1, billboard, particle);
		billboardParticle(vertexIndex + 2, billboard, particle);
		billboardParticle(vertexIndex + 3, billboard, particle);

		quadIndex++;
	}
}

void ParticleEmitter::billboardParticle(unsigned int vertexIndex, const glm::mat4& billboardMat, const Particle* particle)
{
	m_pVertices[vertexIndex].position =
		billboardMat *
		m_pVertices[vertexIndex].position +
		glm::vec4(particle->position, 0);
}

void ParticleEmitter::draw(const Camera& camera)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_mesh.getVBO());
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_firstDeadIndex * 4 * sizeof(Vertex_PositionColor), m_pVertices);

	glUseProgram(m_program.getId());
	m_program.setUniform("projectionView", camera.getProjectionView());
	glBindVertexArray(m_mesh.getVAO());
	glDrawElements(GL_TRIANGLES, m_firstDeadIndex * 6, GL_UNSIGNED_INT, 0);
}
