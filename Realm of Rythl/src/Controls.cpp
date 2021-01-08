#include "Controls.h"
#include "Common.h"



Controls::Controls()
{
    Init();
}

/* Here we initalize variables that are antiquated but still make it clear what the 
 values are referring to before inserting them into a dictionary(buttonMap) which
takes string keys. It stores
*/
void Controls::Init() {
    escape = sf::Keyboard::Escape;
    pause = sf::Keyboard::P;
    toggleFollow = sf::Keyboard::Y;
    toggleTexture = sf::Keyboard::T;
    toggleCollision = sf::Keyboard::C;
    moveUp = sf::Keyboard::W;
    moveLeft = sf::Keyboard::A;
    moveRight = sf::Keyboard::D;
    moveDown = sf::Keyboard::S;
    attack = sf::Keyboard::Space;
    fireArrow = sf::Mouse::Left;

    //Here we insert into the button map the keyword value pairs.
    buttonMap.insert(std::pair < std::string, int>("QUIT",escape));
    buttonMap.insert(std::pair < std::string, int>("PAUSE",pause));
    buttonMap.insert(std::pair < std::string, int>("TOGGLE_FOLLOW",toggleFollow));
    buttonMap.insert(std::pair < std::string, int>("TOGGLE_TEXTURE",toggleTexture));
    buttonMap.insert(std::pair < std::string, int>("TOGGLE_COLLISION",toggleCollision));
    buttonMap.insert(std::pair < std::string, int>("MOVE_UP",moveUp));
    buttonMap.insert(std::pair < std::string, int>("MOVE_LEFT",moveLeft));
    buttonMap.insert(std::pair < std::string, int>("MOVE_DOWN",moveDown));
    buttonMap.insert(std::pair < std::string, int>("MOVE_RIGHT",moveRight));
    buttonMap.insert(std::pair < std::string, int>("ATTACK",attack));
    buttonMap.insert(std::pair < std::string, int>("FIRE_ARROW", fireArrow));
}

void Controls::updateHotkey(std::string buttonToReplace, int newKeyBind)
{
    buttonMap[buttonToReplace] = newKeyBind;

    /*
    std::ofstream controlsFile("Controls.txt", std::ios::trunc);
    
    for (auto Key : buttonMap) {
        controlsFile << Key.first << " " << Key.second << "\n";
            
    }
    */
    // If we want to implement saving settings we just uncomment out this piece here
    // This commented piece of code would allow us to write to a file our current
    // keybindings and would overwrite it everytime in order to avoid any issues with duplicates

}
   
