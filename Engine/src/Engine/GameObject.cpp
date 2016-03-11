#include "GameObject.h"

#include "Light.h"

#include "components/CameraComponent.h"

#include <algorithm>
#include <iostream>


void GameObject::draw(const CameraComponent& camera, const Light& light, const Program& overrideProgram)
{
    glm::vec4 frustum[6];
    camera.getFrustumPlanes(frustum);

    // TODO: Take scale into account
    if (!m_boundingVolume.isInsideFrustum(getTransform().getPosition(), frustum)) {
        std::cout << "Not visible" << std::endl;
        return;
    }

    const float lightOrthoSize = 15;
    glm::mat4 lightProjection = glm::ortho(-lightOrthoSize, lightOrthoSize, -lightOrthoSize, lightOrthoSize, -lightOrthoSize, lightOrthoSize);
    glm::mat4 lightViewInverse = light.getTransform().getInverseMatrix();
    glm::mat4 textureOffsetTransform(0.5f);
    textureOffsetTransform[3] = glm::vec4(0.5f, 0.5f, 0.5f, 1.f);
    glm::mat4 lightProjectionViewNDC = lightProjection * lightViewInverse;
    glm::mat4 lightProjectionView = textureOffsetTransform * lightProjectionViewNDC;

    for (auto& renderable : m_renderables)
    {
        preDraw(camera, light);

        Program program = overrideProgram.isValid() ? overrideProgram : renderable.program;

        glUseProgram(program.getId());
        glPolygonMode(GL_FRONT_AND_BACK, renderable.renderMode);

        // Just blindly go through and set well-known uniforms.
        // TODO: Only do this if needed.

        program.setUniform("modelTransform",  m_transform.getMatrix());
        program.setUniform("viewTransform", camera.getViewTransform());
        program.setUniform("projectionTransform", camera.getProjectionTransform());
        program.setUniform("projectionViewTransform", camera.getProjectionViewTransform());
        program.setUniform("textureOffsetTransform", textureOffsetTransform);
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

        glDrawElements(GL_TRIANGLES, (GLsizei)renderable.mesh.getIndexCount(), GL_UNSIGNED_INT, 0);
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

void GameObject::onKeyEvent(Input::Key key, Input::Action action)
{
    for (auto& component : getComponents())
    {
        component->onKeyEvent(key, action);
    }
}

void GameObject::onMouseButton(Input::MouseButton button, Input::Action action)
{
    for (auto& component : getComponents())
    {
        component->onMouseButton(button, action);
    }
}
