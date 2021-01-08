#pragma once
#include "Common.h"
#include "Components.h"
#include "Physics.h"
#include "EntityManager.h"
#include "Scene.h"
#include "Entity.h"


class AStar
{
private:
    //These are for making the code readable in Controls.cpp
    bool searching = true;

public:



    struct node {
        int posX =0;
        int posY =0;
        int parentX =0;
        int parentY =0;
        int goalCost =0;
        int heurCost =0;
        Vec2 action = Vec2();
        Vec2 size = Vec2();

    };

    EntityManager manager;
    std::vector<struct node> visited = {};
    std::vector<Vec2> visitedPos = {};
    std::vector<struct node> open = {};
    std::vector<Vec2> path = {};
    std::vector<Vec2> TEST = {};
    Vec2 empty = Vec2(0, 0);
    struct node CurrentNode;
    struct node tempNode;
    int goalX =0;
    int goalY =0;
    std::shared_ptr<Entity> goalEntity;
    int counter = 0;
    int counterStored = 0;

    AStar();
    AStar(EntityManager eManager);
    void Init(EntityManager eManager);
    float calculateValue(int xCurrent, int yCurrent, Vec2 size);
    bool isValid(int x, int y, Vec2 action);
    std::vector<Vec2> findPath(std::shared_ptr<Entity> mover, std::shared_ptr<Entity> player);
    bool checkDup(int x, int y);
    bool checkDupByValue(int x, int y, int gPlusH);
    void expand(struct node Node);
    bool isGoal(struct node Node, std::shared_ptr<Entity> player);
    void makeNewNode(struct node parentNode, Vec2 action, int cost);
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;
    bool IsInside(const Vec2& pos, std::shared_ptr<Entity> e);
};
