#include "Scenemanager.h"


void SceneManager::AddScene(SCENEKIND kind, std::shared_ptr<CScene> scene) {
    m_scenes[kind] = scene;
}

void SceneManager::RemoveScene(SCENEKIND kind) {
    m_scenes.erase(kind);
}
