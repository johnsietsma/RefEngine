#include "GameObjectManager.h"

#include "gameObjects/CameraGameObject.h"

bool GameObjectManager::create()
{
    for (auto& gameObject : m_gameObjects)
    {
        if (!gameObject->create())
            return false;
    }
    return true;
}

void GameObjectManager::update( float deltaTime )
{
    for (auto& gameObject : m_gameObjects)
    {
        gameObject->update(deltaTime);
    }
}

void GameObjectManager::draw(const CameraGameObject& camera, const Light* pLight, const Program& overrideProgram, const std::bitset<8>& layers)
{
    for (auto& gameObject : m_gameObjects)
    {
        const size_t layer = gameObject->getLayer();
        if (layer == 0 || layers[layer]) {
            gameObject->draw(camera, pLight, overrideProgram);
        }
    }
}


void GameObjectManager::destroy()
{
    for (auto& gameObject : m_gameObjects)
    {
        gameObject->destroy();
    }
    m_gameObjects.clear();
}

void GameObjectManager::addGameObject(std::shared_ptr<GameObject> pGameObject) 
{ 
    m_gameObjects.push_back(pGameObject); 
}

void GameObjectManager::onKeyEvent(Input::Key key, Input::Action action)
{
    for (auto& pGameObject : m_gameObjects)
    {
        pGameObject->onKeyEvent(key, action);
    }
}

void GameObjectManager::onMouseButton(Input::MouseButton button, Input::Action action)
{
    for (auto& pGameObject : m_gameObjects)
    {
        pGameObject->onMouseButton(button, action);
    }
}
