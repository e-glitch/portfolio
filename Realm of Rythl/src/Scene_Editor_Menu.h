#pragma once

#include "Common.h"
#include "Scene.h"
#include <map>
#include <memory>
#include <deque>


class Scene_Editor_Menu : public Scene
{

protected:

    std::string                 m_title;
    std::string                 m_contextPath;
    std::vector<std::string>    m_menuStrings;
    std::vector<std::string>    m_levelPaths;
    sf::Text                    m_menuText;
    size_t                      m_selectedMenuIndex = 0;

    void init();
    void update();
    void onEnd();
    void sDoAction(const Action& action);

public:

    Scene_Editor_Menu(GameEngine* gameEngine = nullptr);
    void sRender();

};