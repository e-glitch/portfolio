#pragma once

#include "Common.h"
#include "Scene.h"
#include <map>
#include <memory>

#include "EntityManager.h"
#include "Controls.h"
#include "AStar.h"

class Scene_Zelda : public Scene
{

    struct PlayerConfig
    {
        float X = 0, Y= 0, CX = 0, CY =0, SPEED=0, HEALTH=0;
        std::string WEAPON = "";
        bool hasTorch = false;
    };
    typedef std::vector<Vec2> light;

protected:

    Controls m_Controls; // The dictionary string to int which holds the keybinds for the actions
    size_t m_selectedMenuOption = 0; // Currently selected menu option
    float difficulty = 1; // Current difficulty multiplier, used to scale player health and sword damage
    int musicLevel = 25; // Current music sound setting, default is set to 25, goes 0-100. with 0 being mute
    int soundEffectLevel = 25;// Current sound effect setting, default is set to 25, goes 0-100. with 0 being mute
    int m_parallaxXStep = 0;
    int m_parallaxYStep = 0;
    int currentLevel;
    int playerLives = 4;


    int zeroingcurrentframe;


    std::vector<std::string>    m_menuStrings;
    sf::Text                m_menuText;
    std::string             SelectedMenuOption;
    std::shared_ptr<Entity> m_player;
    std::shared_ptr<Entity> m_Boss;
    sf::Vector2f            m_prevView;
    sf::Texture            m_backgroundTex1;
    sf::Texture            m_backgroundTex2;
    sf::Sprite             m_background1;
    sf::Sprite             m_background2;
    std::string             m_levelPath;
    PlayerConfig            m_playerConfig;
    sf::Clock               time;
    bool                    m_drawTextures = true;
    bool                    m_drawCollision = false;
    bool                    m_follow = false;
    // These are all of the menu bools except for m_paused
    bool                    hotkeyMenu = false;
    bool                    assigningHotkey = false;
    bool                    musicVolume = false;
    bool                    soundEffectVolume = false;
    bool                    gameDifficulty = false;
    bool                    inventory = false;

    bool                    bossExists = false;
    sf::Image               baseFilter;
    sf::Image               filterImage;
    light                   lightPixels;


    // HERE IS MY TEST CODE IF THIS IS PUSHED TO GIT DELETE BETWEEN HERE AND NEXT COMMENT

    AStar PathFinder;
    std::vector<Vec2> path;
    // HERE IS MY TEST CODE IF THIS IS PUSHED TO GIT DELETE BETWEEN HERE AND LAST COMMENT

    void init(const std::string & levelPath);

    void loadLevel(const std::string & filename);

    void onEnd();
    void update();
    void spawnPlayer();
    void spawnSword(std::shared_ptr<Entity> entity);
    void spawnArrow(std::shared_ptr<Entity> source, std::shared_ptr<Entity> target);
    void spawnArrowP(std::shared_ptr<Entity> source, Vec2 target);
    Vec2 getPosition(int sx, int sy, int tx, int ty) const;
    
    void sMovement();
    void sAI();
    void sStatus();
    void sAnimation();
    void sCollision();
    void sCamera();
    void OrientateCharacter(std::shared_ptr<Entity> entity, int xDirection , int yDirection );
    void pausedRender();
    void changeDifficulty(std::string newDifficulty);
    void setSoundLevel(int newVolume, std::string SoundType);
    void navigateMenu(std::string newPage);
    void MakeBoss(std::shared_ptr<Entity> newBoss, std::string animation, bool poison, bool gravity, bool moveTile);
    int findCurrentLevel();
    void endLevelVictory(int levelCompleted);
    void addBossPoison(std::shared_ptr<Entity> newBoss);

public:

    Scene_Zelda(GameEngine* gameEngine, const std::string& levelPath);

    void sRender();
    void sDoAction(const Action& action);
};
