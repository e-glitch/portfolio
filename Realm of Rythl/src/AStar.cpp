#include "AStar.h"
#include "Common.h"
#include "Entity.h"

AStar::AStar()
{

}
AStar::AStar(EntityManager eManager)
{
    Init(eManager);
}

void AStar::Init(EntityManager eManager) {
    manager = EntityManager();
    for (auto e : eManager.getEntities())
        if (e->hasComponent<CBoundingBox>()){
            if (e->getComponent<CBoundingBox>().blockMove == true && e->tag() != "NPC") {

                auto entity = manager.addEntity("Block");
                entity->addComponent<CBoundingBox>(e->getComponent<CAnimation>().animation.getSize(), 1, e->getComponent<CBoundingBox>().blockVision);
                entity->addComponent<CTransform>(Vec2(e->getComponent<CTransform>().pos.x, e->getComponent<CTransform>().pos.y));
            }
    }
    manager.update();
}

float AStar::calculateValue(int xCurrent, int yCurrent, Vec2 size)
{

    float xDif = abs(xCurrent - goalX) / (50);
    float yDif = abs(yCurrent - goalY) /(50); // Divided by (size/4)
    float cost = (xDif + yDif) * 100 - std::min(xDif, yDif) * ((2 * 100) - 141);
    return cost;
}
bool AStar::isValid(int x, int y, Vec2 size)
{
    auto entity = manager.addEntity("NewPos");
    entity->addComponent<CBoundingBox>(size, 1, 0);
    entity->addComponent<CTransform>(Vec2(x, y));
    for (auto e : manager.getEntities("Block")) {
        if (Physics::GetOverlap(entity, e) != empty) {
            if (Physics::GetOverlap(goalEntity, entity) != empty) {
                searching = false;
            }
            entity->destroy();
            manager.update();
            return false;
        }
    }
    //std::cout << "Created a node \n";
    entity->destroy();
    manager.update();
    return true;
}
std::vector<Vec2> AStar::findPath(std::shared_ptr<Entity> mover, std::shared_ptr<Entity> player) {



    

    Vec2 mPos = mover->getComponent<CTransform>().pos;
    Vec2 pPos = player->getComponent<CTransform>().pos;

    for (auto e : manager.getEntities("Block")) {
        if (Physics::GetOverlap(e,player) != empty) {
            if (pPos != e->getComponent<CTransform>().pos) {
                //std::cout << "Something else";

            }
            else {
                //std::cout << "Player";
            }
            e->destroy();
        }
        /*
        else if (Physics::GetOverlap(e, mover) != empty) {
                if (mPos == e->getComponent<CTransform>().pos) {
                    e->destroy();
                }
            }*/
        
    }
    manager.update();
    goalX = pPos.x;
    goalY = pPos.y;

    goalEntity = player;



    /*
    x = (mover->getComponent<CTransform>().pos.x - mPos.x) / (mover->getComponent<CBoundingBox>().size.y / 4);
    y = (mover->getComponent<CTransform>().pos.y - mPos.y) / (mover->getComponent<CBoundingBox>().size.y / 4);
    if (x > 1) {
        while (x > 1) {
            path.push_back(Vec2(1, 0));
            x -= 1;
        }
    }
    else if (x < 1) {
        while (x < 1) {
            path.push_back(Vec2(-1, 0));
            x += 1;
        }
    }
    if (y > 1) {
        while (y > 1) {
            path.push_back(Vec2(0, -1));
            y -= 1;
        }
    }
    else if (y < 1) {
        while (y < 1) {
            path.push_back(Vec2(0, 1));
            y += 1;
        }
    }
    //path.push_back(Vec2((mover->getComponent<CTransform>().pos.x -mPos.x)/(mover->getComponent<CBoundingBox>().size.x/4),(mover->getComponent<CTransform>().pos.y -mPos.y)/ (mover->getComponent<CBoundingBox>().size.x / 4)));
    path.push_back(Vec2(x, y));
    auto entity = manager.addEntity("NewPos");
    entity->addComponent<CBoundingBox>(mover->getComponent<CBoundingBox>().size, 1, 0);
    entity->addComponent<CTransform>(mPos);
    for (auto e : manager.getEntities("Block")) {
        if (Physics::GetOverlap(e, entity) != empty) {
            if (pPos != e->getComponent<CTransform>().pos) {
                //std::cout << "a\n";

            }
            else {
                //std::cout << "a\n";
            }
            e->destroy();
        }
    }
    
    if (int(mPos.x - 32) % 64 != 0 || int(mPos.y - 32) % 64 != 0) {
        float x = ((mPos.x - 32) / 64) - roundf((mPos.x - 32) / 64);
        float y = ((mPos.y - 32) / 64) - roundf((mPos.y - 32) / 64);
        x = -(x * 64) / (mover->getComponent<CBoundingBox>().size.x / 4);
        y = -(y * 64) / (mover->getComponent<CBoundingBox>().size.y / 4);


        path.push_back(Vec2(-x, -y));
        //path.push_back(Vec2(-(x*64)/(mover->getComponent<CBoundingBox>().size.x/4),- (y * 64) / (mover->getComponent<CBoundingBox>().size.y / 4)));
        mPos.x = mPos.x - (64/ (mover->getComponent<CBoundingBox>().size.x / 4) * x);
        mPos.y = mPos.y - (64/ (mover->getComponent<CBoundingBox>().size.y / 4) * y);

    }
    */
    float x = (int(mPos.x / 64) * 64) + 32;
    float y = (int(mPos.y / 64) * 64) + 32;
    mPos.x = x;
    mPos.y = y;
    node startNode;
    startNode.posX = mPos.x;
    startNode.posY = mPos.y;
    startNode.goalCost = 0;
    startNode.heurCost = 0;
    startNode.action = Vec2(0,0);
    startNode.size = mover->getComponent<CBoundingBox>().size;
    //path.push_back(Vec2(x, y));

    open.push_back(startNode);


    node goalNode;
    goalNode.posX = goalX;
    goalNode.posY = goalY;

    if (!isValid(mPos.x, mPos.y, startNode.size)) {
        //std::cout << "GOD PLEASE TELL ME WHY";
        path.push_back(Vec2(0, 0));
        return path;

    }
    while (searching) {
        tempNode.heurCost = 9999999;
        counter = 0;
        counterStored = 0;
        for (const auto& nodes : open) {
            if (tempNode.heurCost > nodes.heurCost+nodes.goalCost) {
                CurrentNode = nodes;
                counterStored = counter;
                tempNode.heurCost = nodes.heurCost + nodes.goalCost;
            }
            counter++;
        }
        if (open.size() == 0  ){
            float x = (int(mPos.x / 64) * 64) + 32;
            float y = (int(mPos.y / 64) * 64) + 32;
            mPos.x = x;
            mPos.y = y;
            path.push_back(Vec2(x, y));
            startNode.posX = mPos.x;
            startNode.posY = mPos.y;
            startNode.goalCost = 0;
            startNode.heurCost = 0;
            startNode.action = Vec2(0, 0);
            startNode.size = mover->getComponent<CBoundingBox>().size;
            CurrentNode = startNode;
            open.push_back(startNode);
        }
        if (path.size() == 2 ) {
            path.push_back(Vec2(mPos.x, mPos.y));
            return path;
        }
        open.erase(open.begin()+ counterStored);
        expand(CurrentNode);

        }

    while (!(CurrentNode.parentX == startNode.posX && CurrentNode.parentY == startNode.posY) && !(CurrentNode.posX == startNode.posX && CurrentNode.posY == startNode.posY) && !(CurrentNode.parentX == 0 || CurrentNode.parentY ==0)){
        path.push_back(Vec2(CurrentNode.posX, CurrentNode.posY));

        counter = 0;
        counterStored = 0;
        searching = true;
        for (const auto& nodes : open) {
            
            if (CurrentNode.parentX == nodes.posX && CurrentNode.parentY == nodes.posY && searching) {
                CurrentNode = nodes;
                counterStored = counter;
                searching = false;
            }
            counter++;
        }
        if (!searching) {
            open.erase(open.begin() + counterStored);
        }
        counter = 0;
        counterStored = 0;
        if (searching) {
            for (const auto& nodes : visited) {
                if (CurrentNode.parentX == nodes.posX && CurrentNode.parentY == nodes.posY && searching) {
                    CurrentNode = nodes;
                    counterStored = counter;
                    searching = false;
                }
                counter++;
            }
            if (!searching) {

                visited.erase(visited.begin() + counterStored);
            }
        }

        }
    if (path.size() == 0) {
        path.push_back(Vec2(CurrentNode.posX,CurrentNode.posY));
        open.erase(open.begin());
    }
    if (path.size() >1) {
        if (abs(path[0].x - path[1].x) > 16 || abs(path[0].y - path[1].y) > 16) {
            path.push_back(Vec2(startNode.posX, startNode.posY));

        }
    }
    //std::cout << "One Lap \n \n \n \n";
    return path;

    }
    


bool AStar::checkDup(int x, int y) {
    for (const auto& nodes : open) {
        if (nodes.posX == x && nodes.posY == y) {
            return false;
        }
    }

    for (const auto& nodes : visited) {
        if (nodes.posX == x && nodes.posY == y) {
            return false;
        }
    }
    return true;
}
bool AStar::checkDupByValue(int x, int y, int gPlusH){
    counter = 0;
    counterStored = 0;
for (const auto& nodes : open) {
    
    if (nodes.posX == x && nodes.posY == y) {
        if (gPlusH > nodes.goalCost + nodes.heurCost) {
            return false;
        }
        else {
            open.erase(open.begin() + counter);
        }
    }
    counter += 1;
}
counter = 0;
for (const auto& nodes : visited) {
    if (nodes.posX == x && nodes.posY == y) {
        if (gPlusH > nodes.goalCost + nodes.heurCost) {
            return false;
        }
        else {
            visited.erase(visited.begin() + counter);
        }
    }
    counter += 1;
}
return true;
}

void AStar::expand(node Node){

    visited.push_back(Node);
    if (isValid(Node.posX + (Node.size.x / 4), Node.posY, Node.size) == true) {

        makeNewNode(Node, Vec2(1, 0), 100);
        right = true;

    }
    if (isValid(Node.posX, Node.posY + (Node.size.y / 4), Node.size) == true) {
        makeNewNode(Node, Vec2(0,1), 100);
        up = true;
    }
    if (isValid(Node.posX - (Node.size.x / 4), Node.posY, Node.size) == true) {
        makeNewNode(Node, Vec2(-1, 0), 100);
        left = true;
    }
    if (isValid(Node.posX, Node.posY - (Node.size.y / 4), Node.size) == true) {
        makeNewNode(Node, Vec2(0, -1), 100);
        down = true;
    }



    if (up && right && isValid(Node.posX + Node.size.x / 4, Node.posY + Node.size.y / 4, Node.size) == true) {

        makeNewNode(Node, Vec2(1, 1), 141);

    }
    if (up && left && isValid(Node.posX - Node.size.x / 4, Node.posY + Node.size.y / 4, Node.size) == true) {
        makeNewNode(Node, Vec2(-1, 1), 141);

    }
    if (down && left && isValid(Node.posX - Node.size.x / 4, Node.posY - Node.size.y / 4, Node.size)) {
        makeNewNode(Node, Vec2(-1, -1), 141);
    }
    if (down && right && isValid(Node.posX + Node.size.x / 4, Node.posY - Node.size.y / 4, Node.size)) {
        makeNewNode(Node, Vec2(1, -1), 141);
    }
}


bool AStar::isGoal(node Node, std::shared_ptr<Entity> player) {
    if (Physics::IsInside(Vec2(Node.posX, Node.posY), player)) {
        //std::cout << "Found Goal";
        return true;
    }
    return false;
}

void AStar::makeNewNode(node parentNode, Vec2 action, int cost) {
    tempNode.heurCost = calculateValue(parentNode.posX + ((parentNode.size.x / 4) * action.x), parentNode.posY + ((parentNode.size.y / 4) * action.y), parentNode.size);
    tempNode.posX = parentNode.posX + (parentNode.size.x / 4) * action.x;
    tempNode.posY = parentNode.posY + (parentNode.size.y / 4) * action.y;
    bool dup = checkDupByValue(parentNode.posX + (parentNode.size.x / 4) * action.x, parentNode.posY + (parentNode.size.y / 4) * action.y, tempNode.heurCost + parentNode.goalCost + cost);
    if (dup) {
        tempNode.action = action;
        tempNode.parentX = parentNode.posX;
        tempNode.parentY = parentNode.posY;
        tempNode.size = parentNode.size;
        tempNode.heurCost = calculateValue(parentNode.posX + ((parentNode.size.x / 4) * action.x), parentNode.posY + ((parentNode.size.y / 4) * action.y), parentNode.size);
        tempNode.goalCost = parentNode.goalCost + cost;
        tempNode.posX = parentNode.posX + (parentNode.size.x / 4) * action.x;
        tempNode.posY = parentNode.posY + (parentNode.size.y / 4) * action.y;




        visitedPos.push_back(Vec2(tempNode.parentX, tempNode.parentY));
        if (isGoal(tempNode, goalEntity)) {
            searching = false;
            tempNode.heurCost = -999999999;

        }

        open.push_back(tempNode);
    }
}
bool AStar::IsInside(const Vec2& pos, std::shared_ptr<Entity> e)
{
    // STUDENT TODO:
    auto left = e->getComponent<CTransform>().pos.x - e->getComponent<CBoundingBox>().halfSize.x;
    auto right = e->getComponent<CTransform>().pos.x + e->getComponent<CBoundingBox>().halfSize.x;
    auto top = e->getComponent<CTransform>().pos.y - e->getComponent<CBoundingBox>().halfSize.y;
    auto bottom = e->getComponent<CTransform>().pos.y + e->getComponent<CBoundingBox>().halfSize.y;
    if (pos.x > left-15 && pos.x < right +15 && pos.y > top-15 && pos.y < bottom+15)
    {
        //std::cout << "Found a route \n";
        return true;
    }

    return false;
}