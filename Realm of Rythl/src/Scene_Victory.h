#pragma once

#include "Common.h"
#include "Scene.h"
#include <map>
#include <memory>
#include <deque>

#include "EntityManager.h"

class Scene_Victory : public Scene
{

protected:

    std::string                 m_title;
    std::string                 m_contextPath;
    std::vector<std::string>    m_menuStrings;
    std::vector<std::string>    m_levelPaths;
    sf::Text                    m_menuText;
    size_t                      m_selectedMenuIndex = 0;
    EntityManager               m_entityManager;

    void init();
    void update();
    void onEnd();
    void sDoAction(const Action& action);
    void sAnimation();

public:

    Scene_Victory(GameEngine* gameEngine = nullptr);
    void sRender();

};