#include "GameObject.h"

#include "Camera.h"
#include "Light.h"

#include <iostream>

void GameObject::draw(const Camera& camera, const Light& light)
{
    glm::vec4 frustum[6];
    camera.getFrustumPlanes(frustum);

    for (auto& renderable : m_renderables)
    {
        // TODO: Take scale into account
        if (!m_boundingVolume.isInsideFrustum(getTransform().getPosition(), frustum)) {
            std::cout << "Not visible" << std::endl;
            continue;
        }

        glUseProgram(renderable.program.getId());

        // Just blindly go through and set well-known uniforms.
        // TODO: Only do this if needed.
        renderable.program.setUniform("model",  m_transform.getMatrix());
        renderable.program.setUniform("projectionView", camera.getProjectionView());

        renderable.program.setUniform("lightDirection", light.getTransform().getForward() );
        renderable.program.setUniform("lightColor", light.getColor());
        renderable.program.setUniform("cameraPosition", camera.getTransform().getPosition() );
        renderable.program.setUniform("specularPower", 5);

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
