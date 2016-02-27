#include "GameObject.h"

#include "Camera.h"

#include <iostream>

void GameObject::draw(const Camera& camera)
{
    glm::vec4 frustum[6];
    camera.getFrustumPlanes(frustum);

    for (auto& renderable : m_renderables)
    {
        // TODO: Scale
        if (!m_boundingVolume.isInsideFrustum(getTransform().GetPosition(), frustum)) {
            std::cout << "Not visible" << std::endl;
            continue;
        }

        glUseProgram(renderable.program.getId());

        renderable.program.setUniform("model",  m_transform.GetMatrix());
        renderable.program.setUniform("projectionView", camera.getProjectionView());

        glBindVertexArray(renderable.mesh.getVAO());

        for (const auto& sampler : renderable.samplers)
        {
            GLuint textureId = sampler.texture.getId();
            if (textureId != (GLuint)-1) {
                // Associate the diffuse texture id with the texture unit
                glActiveTexture(GL_TEXTURE0 + sampler.textureUnit);
                glBindTexture(GL_TEXTURE_2D, textureId);
            }
        }

        glDrawElements(GL_TRIANGLES, renderable.mesh.getIndexCount(), GL_UNSIGNED_INT, 0);
    }
}
