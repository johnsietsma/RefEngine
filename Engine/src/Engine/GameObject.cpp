#include "GameObject.h"

#include "Light.h"

#include "gameObjects/CameraGameObject.h"

#include <algorithm>
#include <iostream>


void GameObject::update(float deltaTime) 
{
    for (auto& comp : m_components)
    {
        comp->update(deltaTime);
    }
};


void GameObject::draw(const CameraGameObject& camera, const Light* pLight, const Program& overrideProgram)
{
    glm::vec4 frustum[6];
    camera.getFrustumPlanes(frustum);

    // TODO: Take scale into account
    if (!m_boundingVolume.isInsideFrustum(getTransform().getPosition(), frustum)) {
        //std::cout << "Not visible" << std::endl;
        return;
    }

    glm::mat4 textureOffsetTransform(0.5f);
    textureOffsetTransform[3] = glm::vec4(0.5f, 0.5f, 0.5f, 1.f);

    glm::mat4 lightProjectionViewNDC(1);
    glm::mat4 lightProjectionView(1);


    if (pLight != nullptr) {
        const float lightOrthoSize = 15;
        glm::mat4 lightProjection = glm::ortho(-lightOrthoSize, lightOrthoSize, -lightOrthoSize, lightOrthoSize, -lightOrthoSize, lightOrthoSize);
        glm::mat4 lightViewInverse = pLight->getTransform().getInverseMatrix();
        lightProjectionViewNDC = lightProjection * lightViewInverse;
        lightProjectionView = textureOffsetTransform * lightProjectionViewNDC;
    }

    for (auto& renderable : m_renderables)
    {
        preDraw(camera, pLight);

        Program program = overrideProgram.isValid() ? overrideProgram : renderable.program;

        glUseProgram(program.getId());
        glPolygonMode(GL_FRONT_AND_BACK, renderable.renderMode);

        // Just blindly go through and set well-known uniforms.
        // TODO: Only do this if needed.

        program.setUniform("modelTransform",  m_transform.getMatrix());
        program.setUniform("viewTransform", camera.getViewTransform());
        program.setUniform("projectionTransform", camera.getProjectionTransform());
        program.setUniform("projectionViewTransform", camera.getProjectionViewTransform());
        program.setUniform("lightDirection", pLight->getTransform().getForward() );
        program.setUniform("lightColor", pLight->getColor());
        program.setUniform("cameraPosition", camera.getTransform().getPosition() );
        program.setUniform("specularPower", 5);

        if (pLight != nullptr) {
            program.setUniform("textureOffsetTransform", textureOffsetTransform);
            program.setUniform("lightProjectionViewNDC", lightProjectionViewNDC);
            program.setUniform("lightProjectionView", lightProjectionView);
        }


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
