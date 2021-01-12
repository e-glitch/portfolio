#pragma once

#include "Common.h"
#include "Scene.h"
#include <map>
#include <memory>
#include <deque>

class Scene_Editor : public Scene
{

protected:
    sf::Texture            m_backgroundTex1;
    sf::Sprite             m_background1;
    sf::Texture            m_backgroundTex2;
    sf::Sprite             m_background2;
    sf::Vector2f           m_prevView;
    sf::RectangleShape     m_optionsBox;
    sf::RectangleShape     m_palletRect;
    std::string                 m_title;
    std::string                 m_levelPath;
    std::vector<sf::Text>   m_entityConfigDisplay;
    bool                    m_dark = false;
    bool                    m_drawTextures = true;
    bool                    m_drawCollision = false;
    bool                    m_follow = false;
    bool                    m_drawGrid = true;
    bool                    m_player = false;
    const Vec2              m_gridSize = { 64, 64 };
    sf::Text                m_gridText;
    int m_parallaxXStep = 0;
    int m_parallaxYStep = 0;
    int                     m_xSteps = 0;
    int                     m_ySteps = 0;
    std::vector<std::vector<std::string>>  m_palletNames;
    std::shared_ptr<Entity>   m_selected;
    bool                       m_leftClick = false;
    bool                        m_rightClick = false;
    bool                        m_click = false;
    bool                        m_stepRight =false;
    bool                        m_stepLeft = false;
    bool                        m_stepDown = false;
    bool                        m_stepUp = false;
    bool                        m_toSave = false;
    bool                        m_toPlay = false;
    bool                        m_npc = false;
    sf::Text                    m_editorText;
    std::vector<std::string> m_componentTitles = {"Game Darkness: ", "Tag: ", "CBox Width: ", "CBox Height: ", "Blocks Vision: ", "Blocks Movement: ",
                                                    "Moving Tile: ","Light Source: ", "AI: ","Ranged: ", "Range: ", "Speed: ", "Patrol Points:\n",
                                                    "Patrol Point Tile X: ", "Patrol Point Tile Y: ", "Damage: ", "Health: ", "Boss: "};



    void init(const std::string& levelPath);

    void loadLevel(const std::string& filename);
    void saveLevel(const std::string& filname, EntityManager& entityManager);

    void onEnd();
    void update();
    Vec2 getPosition(int sx, int sy, int tx, int ty) const;
    void updateEntityConfigDisplay();
    void sCamera();
    void sMovement();
    void sCollision();
    
    void drawLine(const Vec2& p1, const Vec2& p2);



public:

    Scene_Editor(GameEngine* gameEngine, const std::string& levelPath);
    void sRender();
    void sDoAction(const Action& action);
};
