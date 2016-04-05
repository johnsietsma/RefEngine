#include "ProceduralGenerationGameObject.h"

#include "engine/Camera.h"
#include "engine/GeometryCreator.h"
#include "engine/ResourceCreator.h"
#include "engine/Transform.h"

#include "graphics/Mesh.h"

#include <glm/gtc/noise.hpp>



ProceduralGenerationGameObject::ProceduralGenerationGameObject(const glm::vec3& pos) :
    GameObject( Transform(pos) )
{
}


ProceduralGenerationGameObject::~ProceduralGenerationGameObject()
{
}

bool ProceduralGenerationGameObject::create()
{
    auto program = ResourceCreator::createProgram("heightMap", "textured");
    if (!program.isValid())
        return false;

    // Create the grid mesh
    Mesh mesh = GeometryCreator::createGrid<Vertex_PositionTexCoord>(100, 100, 0.1f);

    // ---- Create the noise texture ----
    const int rowCount = 64;
    const int columnCount = 64;
    float* pPerlinData = new float[rowCount * columnCount];

    // glm::perlin() will always be 1 when we give it whole numbers. So we scale it down to avoid crossing integer boundaries.
    const int scaleFactor = 6; // How much to strech the coords we give the perlin algorithm.
    const float widthNorm = (1.f / rowCount); // Between 0-1
    const float scale = widthNorm * scaleFactor;

    const int octaves = 8;

    // Fill in the perlin data buffer with noise values
    for (int rowIndex = 0; rowIndex < rowCount; rowIndex++)
    {
        for (int columnIndex = 0; columnIndex < columnCount; columnIndex++)
        {
            const float persistence = 0.3f;
            float amplitude = 0.2f;
            float noiseValue = 0;

            for (int o = 0; o < octaves; o++)
            {
                const float freq = powf(2, static_cast<float>(o));

                // Get a noise value, range is -1 to 1.
                float octaveNoiseValue = glm::perlin(glm::vec2(rowIndex, columnIndex) * scale * freq);

                // Rescale the range from 0 to 1
                octaveNoiseValue = octaveNoiseValue * 0.5f + 0.5f;

                noiseValue += octaveNoiseValue * amplitude;

                amplitude *= persistence;
            }

            pPerlinData[columnIndex * rowCount + rowIndex] = noiseValue;
        }
    }

    m_heightMapTexture.create( pPerlinData, rowCount, columnCount );

    glUseProgram(program.getId());
    program.setUniform("heightMapSampler", m_heightMapTextureUnit);

    delete[] pPerlinData;

    m_diffuseTexture = ResourceCreator::createTexture("./data/textures/Bowling_grass_pxr128.png");
    assert(m_diffuseTexture.isValid());
    program.setUniform("diffuseSampler", m_diffuseTextureUnit);

    std::vector<Sampler> samplers{ Sampler(m_heightMapTexture, 0), Sampler(m_diffuseTexture, 1) };
    m_renderables.emplace_back(program, mesh, samplers);

    return true;
}
