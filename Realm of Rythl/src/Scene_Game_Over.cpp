#include "Scene_Zelda.h"
#include "Scene_Menu.h"
#include "Scene_Victory.h"
#include "Scene_Game_Over.h"
#include "Scene_Map.h"
#include "Common.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"

Scene_Game_Over::Scene_Game_Over(GameEngine* gameEngine)
	: Scene(gameEngine)
{
	init();
}

void Scene_Game_Over::init()
{
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::D, "SELECT");
	registerAction(sf::Keyboard::Escape, "QUIT");

	m_title = "GAME OVER!";
	m_menuStrings.push_back("BACK TO OVERWORLD");
	m_menuStrings.push_back("MAIN MENU");

	m_levelPaths.push_back("campaignLevels/overworld.txt");
	m_levelPaths.push_back("MAIN");

	m_menuText.setFont(m_game->assets().getFont("PiecesOfEight"));
	m_menuText.setCharacterSize(64);
	m_entityManager = EntityManager();

	auto link = m_entityManager.addEntity("NPC");
	link->addComponent<CAnimation>(m_game->assets().getAnimation("LinkDead"), true);
	link->addComponent<CTransform>(Vec2(900, 500));

	m_game->loopSound("MusicVictory");
}

void Scene_Game_Over::update()
{
	m_entityManager.update();
	sAnimation();
}

void Scene_Game_Over::sDoAction(const Action& action)
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
			if (m_menuStrings[m_selectedMenuIndex] == "MAIN MENU")
			{
				m_game->changeScene("MAIN", std::make_shared<Scene_Menu>(m_game));
				m_game->assets().getSound("MusicVictory").stop();
			}
			else
			{
				m_game->changeScene("MAP", std::make_shared<Scene_Map>(m_game, m_levelPaths[m_selectedMenuIndex]));
				m_game->assets().getSound("MusicVictory").stop();
			}
		}
		else if (action.name() == "QUIT")
		{
			onEnd();
		}
	}
}

void Scene_Game_Over::sAnimation()
{
	for (auto e : m_entityManager.getEntities())
	{
		if (e->hasComponent<CAnimation>())
		{
			e->getComponent<CAnimation>().animation.update();

		}
		if (e->getComponent<CAnimation>().animation.hasEnded() && !e->getComponent<CAnimation>().repeat) {
			e->destroy();
		}
	}
}

void Scene_Game_Over::sRender()
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
	m_menuText.setString("UP: W     DOWN: S    SELECT: D      EXIT: ESC");
	m_menuText.setPosition(sf::Vector2f(10, 690));
	m_game->window().draw(m_menuText);


	for (auto e : m_entityManager.getEntities("NPC"))
	{
		auto& transform = e->getComponent<CTransform>();
		sf::Color c = sf::Color::White;
		auto& animation = e->getComponent<CAnimation>().animation;
		animation.getSprite().setRotation(transform.angle);
		animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
		animation.getSprite().setScale(transform.scale.x, transform.scale.y);
		animation.getSprite().setColor(c);
		m_game->window().draw(animation.getSprite());
	}
}

void Scene_Game_Over::onEnd()
{
	m_hasEnded = true;
	m_game->quit();
}