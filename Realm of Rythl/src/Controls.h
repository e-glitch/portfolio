#ifndef Controls_h
#define Controls_h
#include <map>
#include "Common.h"


class Controls
{
private:
    //These are for making the code readable in Controls.cpp
    int escape;
    int pause;
    int toggleFollow ;
    int toggleTexture ;
    int toggleCollision ;
    int moveUp;
    int moveLeft;
    int moveRight ;
    int moveDown;
    int attack;
    int fireArrow;



public:
    
    std::map<std::string,int> buttonMap;
    Controls();
    void Init();
    void updateHotkey(std::string buttonToReplace, int newKeyBind);
};

#endif
