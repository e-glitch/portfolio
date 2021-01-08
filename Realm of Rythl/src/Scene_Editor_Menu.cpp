#include "Scene_Menu.h"
#include "Scene_Editor_Menu.h"
#include "Scene_Editor.h"
#include "Common.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Action.h"

Scene_Editor_Menu::Scene_Editor_Menu(GameEngine* gameEngine)
    : Scene(gameEngine)
{
    init();
}

void Scene_Editor_Menu::init()
{
    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::D, "SELECT");
    registerAction(sf::Keyboard::Escape, "QUIT");




    m_title = "Select A Level To Edit";
    m_contextPath = "customLevels/";
    m_menuStrings.push_back("Level  1");
    m_menuStrings.push_back("Level  2");
    m_menuStrings.push_back("Level  3");
    m_menuStrings.push_back("Level  4");
    m_menuStrings.push_back("Level  5");
    m_menuStrings.push_back("Level  6");
    m_menuStrings.push_back("Level  7");
    m_menuStrings.push_back("Level  8");
    m_menuStrings.push_back("Level  9");
    m_menuStrings.push_back("Back");

    m_levelPaths.push_back(m_contextPath + "level1.txt");
    m_levelPaths.push_back(m_contextPath + "level2.txt");
    m_levelPaths.push_back(m_contextPath + "level3.txt");
    m_levelPaths.push_back(m_contextPath + "level4.txt");
    m_levelPaths.push_back(m_contextPath + "level5.txt");
    m_levelPaths.push_back(m_contextPath + "level6.txt");
    m_levelPaths.push_back(m_contextPath + "level7.txt");
    m_levelPaths.push_back(m_contextPath + "level8.txt");
    m_levelPaths.push_back(m_contextPath + "level9.txt");

    m_menuText.setFont(m_game->assets().getFont("PiecesOfEight"));
    m_menuText.setCharacterSize(64);

    m_game->playSound("MusicTitle");
}

void Scene_Editor_Menu::update()
{
    m_entityManager.update();
}

void Scene_Editor_Menu::sDoAction(const Action& action)
{
    if (action.type() == "START")
    {
        if (action.name() == "UP")
        {
            if (m_selectedMenuIndex > 0) { m_selectedMenuIndex--; }
            else { m_selectedMenuIndex = m_menuStrings.size() - 1; }
        }
        else if (action.name() == "DOWN")
        {
            m_selectedMenuIndex = (m_selectedMenuIndex + 1) % m_menuStrings.size();
        }
        else if (action.name() == "SELECT")
        {
            if (m_menuStrings[m_selectedMenuIndex] == "Back")
            {
                m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game));
            }
            else
            {
                m_game->changeScene("EDITOR", std::make_shared<Scene_Editor>(m_game, m_levelPaths[m_selectedMenuIndex]));
            }
            
            m_game->assets().getSound("MusicTitle").stop();
        }
        else if (action.name() == "QUIT")
        {
            m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game));
        }
    }
}

void Scene_Editor_Menu::sRender()
{
    // clear the window to a blue
    m_game->window().setView(m_game->window().getDefaultView());
    m_game->window().clear(sf::Color(0, 0, 0));

    // draw the game title in the top-left of the screen
    m_menuText.setCharacterSize(48);
    m_menuText.setString(m_title);
    m_menuText.setFillColor(sf::Color(100, 100, 100));
    m_menuText.setPosition(sf::Vector2f(10, 10));
    m_game->window().draw(m_menuText);

    // draw all of the menu options
    for (size_t i = 0; i < m_menuStrings.size(); i++)
    {
        m_menuText.setString(m_menuStrings[i]);
        m_menuText.setFillColor(i == m_selectedMenuIndex ? sf::Color::White : sf::Color(100, 100, 100));
        if (i < 5)
        {
            m_menuText.setPosition(sf::Vector2f(10, 110 + i * 72));
        }
        else
        {
            m_menuText.setPosition(sf::Vector2f(650, 110 + (i - 5)* 72));
        }
        m_game->window().draw(m_menuText);
    }

    // draw the controls in the bottom-left
    m_menuText.setCharacterSize(20);
    m_menuText.setFillColor(sf::Color(100, 100, 100));
    m_menuText.setString("up: w     down: s    play: d      back: esc");
    m_menuText.setPosition(sf::Vector2f(10, 690));
    m_game->window().draw(m_menuText);
}

void Scene_Editor_Menu::onEnd()
{
    m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game));
}