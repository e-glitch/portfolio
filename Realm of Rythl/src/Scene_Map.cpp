#include "Scene_Map.h"
#include "Scene_Zelda.h"
#include "Scene_Menu.h"
#include "Common.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include <vector>
#include <cmath>
#include <filesystem>

Scene_Map::Scene_Map(GameEngine* game, const std::string& levelPath)
	: Scene(game)
	, m_levelPath(levelPath)
{
	init(m_levelPath);
}


void Scene_Map::init(const std::string& levelPath)
{
	loadSave();
	loadLevel(levelPath);

	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::Y, "TOGGLE_FOLLOW");      // Toggle drawing (T)extures
	registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");      // Toggle drawing (T)extures
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");    // Toggle drawing (C)ollision Boxes
	registerAction(sf::Keyboard::W, "MOVE_UP");             // Movement actions
	registerAction(sf::Keyboard::A, "MOVE_LEFT");
	registerAction(sf::Keyboard::S, "MOVE_DOWN");
	registerAction(sf::Keyboard::D, "MOVE_RIGHT");
	registerAction(sf::Keyboard::Space, "START_LEVEL");
}

void Scene_Map::loadSave()
{
	std::ifstream inFile("LevelsCompleted.txt");
	std::string line;
	std::getline(inFile, line);
	std::istringstream iss(line);
	if (iss >> line)
	{
		m_levelsCompleted = std::stoi(line);
	}
}

void Scene_Map::loadLevel(const std::string& filename)
{
	m_entityManager = EntityManager();

	std::ifstream inFile(filename);
	std::string line;
	std::vector<std::string> returnString;
	std::vector<std::string> fileLines;

	while (std::getline(inFile, line))
	{
		returnString.push_back(line);
	}

	for (size_t i = 0; i < returnString.size(); i++)
	{

		std::istringstream iss(returnString[i]);
		// if blank line, skip
		if (!(iss >> line))
		{
			continue;
		}
		if (line == "Level")
		{
			std::string animation, fileName;
			int roomX;
			int roomY;
			int tileX;
			int tileY;

			auto t = m_entityManager.addEntity("level");
			iss >> animation >> roomX >> roomY >> tileX >> tileY >> fileName;
			t->addComponent<CAnimation>(m_game->assets().getAnimation(animation), true);
			t->addComponent<CTransform>(getPosition(roomX, roomY, tileX, tileY));
			t->addComponent<CBoundingBox>(m_game->assets().getAnimation(animation).getSize(), false, false);
			t->addComponent<CLevelPath>(fileName);

		}
		if (line == "Tile")
		{
			std::string animation;
			int roomX;
			int roomY;
			int tileX;
			int tileY;
			bool blockMove;
			bool blockVis;

			auto t = m_entityManager.addEntity("tile");
			iss >> animation >> roomX >> roomY >> tileX >> tileY >> blockMove >> blockVis;
			t->addComponent<CAnimation>(m_game->assets().getAnimation(animation), true);
			t->addComponent<CTransform>(getPosition(roomX, roomY, tileX, tileY));
			t->addComponent<CBoundingBox>(m_game->assets().getAnimation(animation).getSize(), blockMove, blockVis);

		}
		if (line == "Player")
		{
			int numLevels;
			float spawnX, spawnY, characterWidth, characterHeight;
			float speed;
			std::vector<Vec2> levelCoords;

			iss >> spawnX >> spawnY >> characterWidth >> characterHeight >> speed >> numLevels;

			m_playerConfig.X = spawnX;
			m_playerConfig.Y = spawnY;
			m_playerConfig.CX = characterWidth;
			m_playerConfig.CY = characterHeight;
			m_playerConfig.SPEED = speed;
			// if the player is playing campaign levels, limit their movement so they can't skip levels
			// " + 2" so that the player can always move between the town and first level
			// if the map is for the custom levels, give it no limitation so that the player can play all custom levels

			if (filename != "customLevels/overworld.txt") {
				if (m_levelsCompleted + 2 < numLevels)
				{
					numLevels = m_levelsCompleted + 2;
				}
			}
			for (int i = 0; i < numLevels; i++)
			{
				int x, y;
				iss >> x >> y;
				levelCoords.push_back(Vec2(x, y));
			}

			m_playerConfig.PATH = levelCoords;
		}
	}
	m_game->loopSound("MusicMap");
	spawnPlayer();
}

Vec2 Scene_Map::getPosition(int rx, int ry, int tx, int ty) const
{
	return Vec2((rx * 1280) + ((tx + 0.5) * 64), (ry * 768) + ((ty + 0.5) * 64));
}

void Scene_Map::spawnPlayer()
{
	m_player = m_entityManager.addEntity("player");
	m_player->addComponent<CTransform>(Vec2(m_playerConfig.X, m_playerConfig.Y));
	m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandDown"), true);
	m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY), true, false);
	m_player->addComponent<CFollowPath>(m_playerConfig.PATH);
	m_player->addComponent<CInput>();
}

void Scene_Map::update()
{
	if (!m_paused) {
		m_entityManager.update();

		sMovement();
		sCollision();
		sAnimation();
		sCamera();

		m_currentFrame++;
	}
}

void Scene_Map::sMovement()
{
	// boolean math to determine speed
	// if opposite directions held, 1-1 = 0 * SPEED = 0. Same if neither direction is held (0-0 = 0). 
	// If one direction is held, it is 1*SPEED or -1*SPEED, to determine direction.
	if (abs(m_player->getComponent<CTransform>().velocity.x) < abs(m_player->getComponent<CTransform>().velocity.y)) {

		if (m_player->getComponent<CTransform>().velocity.y > 0) {
			m_player->getComponent<CState>().state = "MovingDown";
		}
		if (m_player->getComponent<CTransform>().velocity.y < 0) {
			m_player->getComponent<CState>().state = "MovingUp";

		}
	}
	else {
		if (m_player->getComponent<CTransform>().velocity.x > 0) {
			m_player->getComponent<CState>().state = "MovingRight";
		}
		if (m_player->getComponent<CTransform>().velocity.x < 0) {
			m_player->getComponent<CState>().state = "MovingLeft";
		}
	}
	if (m_player->getComponent<CTransform>().velocity.x == 0 && m_player->getComponent<CTransform>().velocity.y == 0) {
		if (m_player->getComponent<CState>().state == "MovingDown") {
			m_player->getComponent<CState>().state = "StandingDown";
		}
		if (m_player->getComponent<CState>().state == "MovingUp") {
			m_player->getComponent<CState>().state = "StandingUp";
		}
		if (m_player->getComponent<CState>().state == "MovingRight") {
			m_player->getComponent<CState>().state = "StandingRight";
		}
		if (m_player->getComponent<CState>().state == "MovingLeft") {
			m_player->getComponent<CState>().state = "StandingLeft";
		}
	}

	if (m_player->hasComponent<CFollowPath>() && m_player->getComponent<CFollowPath>().positions.size() != 0) {
		auto nPos = m_player->getComponent<CTransform>().pos;
		auto nFollowPathPos = m_player->getComponent<CFollowPath>().positions[m_player->getComponent<CFollowPath>().currentPosition];
		// if the player isn't in range of their destination, path to it
		// uses mostly tha same code as the patrol AI, but with a changing "home"
		if (abs(nPos.x - nFollowPathPos.x) > 3
			|| abs(nPos.y - nFollowPathPos.y) > 3)
		{
			m_player->getComponent<CTransform>().velocity = pathfind(nPos, nFollowPathPos, m_playerConfig.SPEED);
		}
		else
		{
			m_player->getComponent<CTransform>().velocity = Vec2(0, 0);
			if (m_player->getComponent<CInput>().right || m_player->getComponent<CInput>().up) {
				if (m_player->getComponent<CFollowPath>().currentPosition < m_player->getComponent<CFollowPath>().positions.size() - 1)
				{
					m_player->getComponent<CFollowPath>().currentPosition++;
				}
			}
			else if (m_player->getComponent<CInput>().left || m_player->getComponent<CInput>().down) {
				if (m_player->getComponent<CFollowPath>().currentPosition > 0)
				{
					m_player->getComponent<CFollowPath>().currentPosition--;
				}
			}
		}
	}


	for (auto e : m_entityManager.getEntities())
	{
		if (e->hasComponent<CTransform>())
		{
			auto transform = e->getComponent<CTransform>();
			transform.prevPos = transform.pos;
			e->getComponent<CTransform>().pos.x = transform.pos.x + transform.velocity.x;
			e->getComponent<CTransform>().pos.y = transform.pos.y + transform.velocity.y;
		}
	}
}

void Scene_Map::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{

		if (action.name() == "QUIT") { onEnd(); }
		else if (action.name() == "TOGGLE_FOLLOW") { m_follow = !m_follow; }
		else if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
		else if (action.name() == "MOVE_UP") { m_player->getComponent<CInput>().up = true; }
		else if (action.name() == "MOVE_LEFT") { m_player->getComponent<CInput>().left = true; }
		else if (action.name() == "MOVE_DOWN") { m_player->getComponent<CInput>().down = true; }
		else if (action.name() == "MOVE_RIGHT") { m_player->getComponent<CInput>().right = true; }
		else if (action.name() == "START_LEVEL") { m_player->getComponent<CInput>().attack = true; }
	}
	else if (action.type() == "END")
	{
		if (action.name() == "MOVE_UP") { m_player->getComponent<CInput>().up = false; }
		else if (action.name() == "MOVE_LEFT") { m_player->getComponent<CInput>().left = false; }
		else if (action.name() == "MOVE_DOWN") { m_player->getComponent<CInput>().down = false; }
		else if (action.name() == "MOVE_RIGHT") { m_player->getComponent<CInput>().right = false; }
		else if (action.name() == "START_LEVEL") { m_player->getComponent<CInput>().attack = false; }
	}
}

Vec2 Scene_Map::pathfind(Vec2 currentPos, Vec2 destination, float speed)
{
	// dumb pathfinding towards a destination
	// used to get from one point to another in a straight line
	float xDist = currentPos.x - destination.x;
	float yDist = currentPos.y - destination.y;
	float percentageX = xDist / (abs(xDist) + abs(yDist));
	float percentageY = yDist / (abs(xDist) + abs(yDist));
	return Vec2(-percentageX * speed, -percentageY * speed);
}

void Scene_Map::sCollision()
{
	if (m_player->getComponent<CInput>().attack)
	{
		for (auto l : m_entityManager.getEntities("level"))
		{
			if (Physics::IsInside(m_player->getComponent<CTransform>().pos, l))
			{
				m_game->assets().getSound("MusicMap").stop();
				m_game->changeScene("ZELDA", std::make_shared<Scene_Zelda>(m_game, l->getComponent<CLevelPath>().path));
			}
		}
	}
}

void Scene_Map::sAnimation()
{
	auto playerVelocity = m_player->getComponent<CTransform>().velocity;
	auto playerState = m_player->getComponent<CState>().state;
	auto playerAnimation = m_player->getComponent<CAnimation>().animation.getName();
	if (playerState == "MovingRight" && (playerAnimation != "RunRight" || (m_player->getComponent<CTransform>().scale.x == -1 && playerAnimation == "RunRight")))
	{
		m_player->addComponent<CAnimation>(m_game->assets().getAnimation("RunRight"), true);
		m_player->getComponent<CTransform>().scale.x = 1;
		m_player->getComponent<CTransform>().facing.x = 1;
		OrientateCharacter(m_player, 1, 0);
	}

	else if (playerState == "MovingLeft" && (playerAnimation != "RunRight" || (m_player->getComponent<CTransform>().scale.x == 1 && playerAnimation == "RunRight")))
	{
		m_player->addComponent<CAnimation>(m_game->assets().getAnimation("RunRight"), true);

		OrientateCharacter(m_player, -1, 0);
	}

	else if (playerState == "MovingDown" && playerAnimation != "RunDown")
	{
		m_player->addComponent<CAnimation>(m_game->assets().getAnimation("RunDown"), true);
		OrientateCharacter(m_player, 1, -1);
	}
	else if (playerState == "MovingUp" && playerAnimation != "RunUp")
	{
		m_player->addComponent<CAnimation>(m_game->assets().getAnimation("RunUp"), true);
		OrientateCharacter(m_player, 1, 1);
	}
	else if (playerState == "StandingLeft") {
		m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandRight"), true);
		m_player->getComponent<CTransform>().scale.x = -1;
		OrientateCharacter(m_player, -1, 0);
	}
	else if (playerState == "StandingRight") {
		m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandRight"), true);
		m_player->getComponent<CTransform>().scale.x = 1;
		OrientateCharacter(m_player, 1, 0);
	}
	else if (playerState == "StandingUp") {
		m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandUp"), true);
		OrientateCharacter(m_player, 1, 1);
	}
	else if (playerState == "StandingDown") {
		m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandDown"), true);
		OrientateCharacter(m_player, 1, -1);
	}

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

void Scene_Map::OrientateCharacter(std::shared_ptr<Entity> entity, int xDirection = 1, int yDirection = 0) {
	entity->getComponent<CTransform>().scale.x = xDirection;
	entity->getComponent<CTransform>().facing.x = xDirection;
	entity->getComponent<CTransform>().facing.y = yDirection;
}

void Scene_Map::sCamera()
{

	// get the current view, which we will modify in the if-statement below
	sf::View view = m_game->window().getView();

	// calculate view for player follow camera
	view.setCenter(m_player->getComponent<CTransform>().pos.x, m_player->getComponent<CTransform>().pos.y);

	// then set the window view
	m_game->window().setView(view);
}

void Scene_Map::onEnd()
{
	m_game->assets().getSound("MusicMap").stop();
	m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game));
	m_game->loopSound("MusicTitle");
}

void Scene_Map::sRender()
{
	// RENDERING DONE FOR YOU

	m_game->window().clear(sf::Color(255, 192, 122));
	sf::RectangleShape tick({ 1.0f, 6.0f });
	tick.setFillColor(sf::Color::Black);

	// draw all Entity textures / animations
	if (m_drawTextures)
	{
		for (auto e : m_entityManager.getEntities())
		{
			auto& transform = e->getComponent<CTransform>();
			sf::Color c = sf::Color::White;
			if (e->hasComponent<CInvincibility>())
			{
				c = sf::Color(255, 255, 255, 128);
			}

			if (e->hasComponent<CAnimation>())
			{
				auto& animation = e->getComponent<CAnimation>().animation;
				animation.getSprite().setRotation(transform.angle);
				animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
				animation.getSprite().setScale(transform.scale.x, transform.scale.y);
				animation.getSprite().setColor(c);
				m_game->window().draw(animation.getSprite());
			}
		}

		for (auto e : m_entityManager.getEntities())
		{
			auto& transform = e->getComponent<CTransform>();
			if (e->hasComponent<CHealth>())
			{
				auto& h = e->getComponent<CHealth>();
				Vec2 size(64, 6);
				sf::RectangleShape rect({ size.x, size.y });
				rect.setPosition(transform.pos.x - 32, transform.pos.y - 48);
				rect.setFillColor(sf::Color(96, 96, 96));
				rect.setOutlineColor(sf::Color::Black);
				rect.setOutlineThickness(2);
				m_game->window().draw(rect);

				float ratio = (float)(h.current) / h.max;
				size.x *= ratio;
				rect.setSize({ size.x, size.y });
				rect.setFillColor(sf::Color(255, 0, 0));
				rect.setOutlineThickness(0);
				m_game->window().draw(rect);

				for (int i = 0; i < h.max; i++)
				{
					tick.setPosition(rect.getPosition() + sf::Vector2f(i * 64 * 1 / h.max, 0));
					m_game->window().draw(tick);
				}
			}
		}
	}

	// draw all Entity collision bounding boxes with a rectangleshape
	if (m_drawCollision)
	{
		sf::CircleShape dot(4);
		dot.setFillColor(sf::Color::Black);
		for (auto e : m_entityManager.getEntities())
		{
			if (e->hasComponent<CBoundingBox>())
			{
				auto& box = e->getComponent<CBoundingBox>();
				auto& transform = e->getComponent<CTransform>();
				sf::RectangleShape rect;
				rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
				rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
				rect.setPosition(transform.pos.x, transform.pos.y);
				rect.setFillColor(sf::Color(0, 0, 0, 0));

				if (box.blockMove && box.blockVision) { rect.setOutlineColor(sf::Color::Black); }
				if (box.blockMove && !box.blockVision) { rect.setOutlineColor(sf::Color::Blue); }
				if (!box.blockMove && box.blockVision) { rect.setOutlineColor(sf::Color::Red); }
				if (!box.blockMove && !box.blockVision) { rect.setOutlineColor(sf::Color::White); }
				rect.setOutlineThickness(1);
				m_game->window().draw(rect);
			}

			if (e->hasComponent<CPatrol>())
			{
				auto& patrol = e->getComponent<CPatrol>().positions;
				for (size_t p = 0; p < patrol.size(); p++)
				{
					dot.setPosition(patrol[p].x, patrol[p].y);
					m_game->window().draw(dot);
				}
			}

			if (e->hasComponent<CFollowPlayer>())
			{
				sf::VertexArray lines(sf::LinesStrip, 2);
				lines[0].position.x = e->getComponent<CTransform>().pos.x;
				lines[0].position.y = e->getComponent<CTransform>().pos.y;
				lines[0].color = sf::Color::Black;
				lines[1].position.x = m_player->getComponent<CTransform>().pos.x;
				lines[1].position.y = m_player->getComponent<CTransform>().pos.y;
				lines[1].color = sf::Color::Black;
				m_game->window().draw(lines);
				dot.setPosition(e->getComponent<CFollowPlayer>().home.x, e->getComponent<CFollowPlayer>().home.y);
				m_game->window().draw(dot);
			}
		}
	}
}
