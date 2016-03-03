#include "GameObject.h"

#include "Camera.h"
#include "Light.h"

#include <algorithm>
#include <iostream>


void GameObject::draw(const Camera& camera, const Light& light, Program overrideProgram)
{
    glm::vec4 frustum[6];
    camera.getFrustumPlanes(frustum);

    // TODO: Take scale into account
    if (!m_boundingVolume.isInsideFrustum(getTransform().getPosition(), frustum)) {
        std::cout << "Not visible" << std::endl;
        return;
    }

    glm::mat4 lightProjection = glm::ortho<float>(-10, 10, -10, 10, -10, 10);
    glm::mat4 lightView = light.getTransform().getInverseMatrix();
    glm::mat4 textureSpaceOffset(0.5f);
    textureSpaceOffset[3] = glm::vec4(0.5f, 0.5f, 0.5f, 1.f);
    glm::mat4 lightProjectionViewNDC = lightProjection * lightView;
    glm::mat4 lightProjectionView = textureSpaceOffset * lightProjectionViewNDC;

    for (auto& renderable : m_renderables)
    {
        preDraw(camera, light);

        Program program = overrideProgram.isValid() ? overrideProgram : renderable.program;

        glUseProgram(program.getId());
        glPolygonMode(GL_FRONT_AND_BACK, renderable.renderMode);

        // Just blindly go through and set well-known uniforms.
        // TODO: Only do this if needed.

        program.setUniform("model",  m_transform.getMatrix());
        program.setUniform("projectionView", camera.getProjectionView());
        program.setUniform("lightDirection", light.getTransform().getForward() );
        program.setUniform("lightColor", light.getColor());
        program.setUniform("lightProjectionViewNDC", lightProjectionViewNDC);
        program.setUniform("lightProjectionView", lightProjectionView);
        program.setUniform("cameraPosition", camera.getTransform().getPosition() );
        program.setUniform("specularPower", 5);

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

void GameObject::destroy()
{
    for (auto& renderable : m_renderables) {
        renderable.program.destroy();
        renderable.mesh.destroy();
        for (auto& sampler : renderable.samplers) {
            sampler.texture.destroy();
        }
    }
    m_renderables.clear();
}