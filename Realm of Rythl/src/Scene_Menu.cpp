#include "Scene_Menu.h"
#include "Scene_Zelda.h"
#include "Scene_Victory.h"
#include "Scene_Map.h"
#include "Common.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"
#include "Scene_Editor_Menu.h"

Scene_Menu::Scene_Menu(GameEngine* gameEngine)
	: Scene(gameEngine)
{
	init();
}

void Scene_Menu::init()
{
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::D, "ZELDA");
	registerAction(sf::Keyboard::Escape, "QUIT");

	m_title = "REALM OF RYTHL";
	m_menuStrings.push_back("OVERWORLD");
	m_menuStrings.push_back("CUSTOM LEVELS");
	m_menuStrings.push_back("LEVEL EDITOR");

	m_levelPaths.push_back("campaignLevels/overworld.txt");
	m_levelPaths.push_back("customLevels/overworld.txt");
	m_levelPaths.push_back("Editor");

	m_menuText.setFont(m_game->assets().getFont("PiecesOfEight"));
	m_menuText.setCharacterSize(64);

	m_game->loopSound("MusicTitle");
}

void Scene_Menu::update()
{
	m_entityManager.update();
}

void Scene_Menu::sDoAction(const Action& action)
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
		else if (action.name() == "ZELDA")
		{
			if (m_menuStrings[m_selectedMenuIndex] == "LEVEL EDITOR")
			{
				m_game->changeScene("Editor", std::make_shared<Scene_Editor_Menu>(m_game));
				m_game->assets().getSound("MusicTitle").stop();
			}
			else
			{
				m_game->changeScene("MAP", std::make_shared<Scene_Map>(m_game, m_levelPaths[m_selectedMenuIndex]));
				m_game->assets().getSound("MusicTitle").stop();
			}
		}
		else if (action.name() == "QUIT")
		{
			onEnd();
		}
	}
}

void Scene_Menu::sRender()
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
		m_menuText.setPosition(sf::Vector2f(10, 110 + i * 72));
		m_game->window().draw(m_menuText);
	}

	// draw the controls in the bottom-left
	m_menuText.setCharacterSize(20);
	m_menuText.setFillColor(sf::Color(100, 100, 100));
	m_menuText.setString("UP: W     DOWN: S    PLAY: D      BACK: ESC");
	m_menuText.setPosition(sf::Vector2f(10, 690));
	m_game->window().draw(m_menuText);
}

void Scene_Menu::onEnd()
{
	m_hasEnded = true;
	m_game->quit();
}