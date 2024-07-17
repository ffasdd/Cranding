#pragma once
#include <map>
#include "stdafx.h"
#include "Scene.h"

class CScene;

class SceneManager
{
public:

    static SceneManager& GetInstance() {
        static SceneManager instance;
        return instance;
    }
    SceneManager() {} 
    ~SceneManager() {}

    void AddScene(SCENEKIND kind, std::shared_ptr<CScene> scene);
    void RemoveScene(SCENEKIND kind);


    SCENEKIND GetCurrentScene() const { return m_nCurScene; }
    void SetCurrentScene(SCENEKIND kind) { m_nCurScene = kind; }

private:
    std::map<SCENEKIND, std::shared_ptr<CScene>> m_scenes;
    std::shared_ptr<CScene> m_currentScene;
    SCENEKIND m_nCurScene = SCENEKIND::LOGIN;
};

