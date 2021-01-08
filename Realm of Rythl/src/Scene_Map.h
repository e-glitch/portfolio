#pragma once

#include "Common.h"
#include "Scene.h"
#include <map>
#include <memory>

#include "EntityManager.h"

class Scene_Map : public Scene
{

    struct PlayerConfig
    {
        float X = 0, Y = 0, CX = 0, CY = 0, SPEED = 0;
        std::vector<Vec2> PATH = {};
    };

protected:

    std::shared_ptr<Entity> m_player;
    std::string             m_levelPath;
    PlayerConfig            m_playerConfig;
    bool                    m_drawTextures = true;
    bool                    m_drawCollision = false;
    bool                    m_follow = false;
    int                     m_levelsCompleted = 0;

    void init(const std::string& levelPath);

    void loadLevel(const std::string& filename);
    void loadSave();

    void onEnd();
    void update();
    void spawnPlayer();
    Vec2 getPosition(int sx, int sy, int tx, int ty) const;
    Vec2 pathfind(Vec2 currentPos, Vec2 dest, float speed);

    void sMovement();
    void sAnimation();
    void sCollision();
    void sCamera();
    void OrientateCharacter(std::shared_ptr<Entity> entity, int xDirection, int yDirection);


public:

    Scene_Map(GameEngine* gameEngine, const std::string& levelPath);

    void sRender();
    void sDoAction(const Action& action);
};