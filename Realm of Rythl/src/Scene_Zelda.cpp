/* Authors:
*		Brandon Mertes 		201618386
*		Erika James 		201602265
*		Keegan Churchill 	201510260
*		Mackenzie Barrett 	201519360
*
*/
#include "Scene_Zelda.h"
#include "Scene_Menu.h"
#include "Scene_Victory.h"
#include "Scene_Game_Over.h"
#include "Scene_Map.h"
#include "Common.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include <vector>
#include <cmath>
#include "Controls.h"

Scene_Zelda::Scene_Zelda(GameEngine* game, const std::string& levelPath)
	: Scene(game)
	, m_levelPath(levelPath)
{
	init(m_levelPath);
}


void Scene_Zelda::init(const std::string& levelPath)
{
	loadLevel(levelPath);

	// STUDENT TODO:
	// Register the actions required to play the game

	// Have added a class that will store int variables corresponding to the
	// sf::keyboard library so that we can change them as needed

	m_Controls = Controls();
	m_follow = true;
	registerAction(m_Controls.buttonMap["QUIT"], "QUIT");
	registerAction(m_Controls.buttonMap["PAUSE"], "PAUSE");
	registerAction(m_Controls.buttonMap["TOGGLE_FOLLOW"], "TOGGLE_FOLLOW");      // Toggle drawing (T)extures
	registerAction(m_Controls.buttonMap["TOGGLE_TEXTURE"], "TOGGLE_TEXTURE");      // Toggle drawing (T)extures
	registerAction(m_Controls.buttonMap["TOGGLE_COLLISION"], "TOGGLE_COLLISION");    // Toggle drawing (C)ollision Boxes
	registerAction(m_Controls.buttonMap["MOVE_UP"], "MOVE_UP");             // Movement actions
	registerAction(m_Controls.buttonMap["MOVE_LEFT"], "MOVE_LEFT");
	registerAction(m_Controls.buttonMap["MOVE_DOWN"], "MOVE_DOWN");
	registerAction(m_Controls.buttonMap["MOVE_RIGHT"], "MOVE_RIGHT");
	registerAction(m_Controls.buttonMap["ATTACK"], "ATTACK");
	registerMouseButtonAction(m_Controls.buttonMap["FIRE_ARROW"], "FIRE_ARROW");

	//Create ParallaxBackground
	//Background 1
	m_backgroundTex1.loadFromFile("images/tilesets/clouds.png");
	m_backgroundTex1.setRepeated(true);
	m_background1.setTextureRect(sf::IntRect(0, 0, 2000, 1200));
	m_background1.setTexture(m_backgroundTex1);
	m_background1.setPosition(sf::Vector2f(-400, -200));
	//Background 2
	m_backgroundTex2.loadFromFile("images/tilesets/floatingIslands.png");
	m_backgroundTex2.setRepeated(true);
	m_background2.setTextureRect(sf::IntRect(0, 0, 1300, 800));
	m_background2.setTexture(m_backgroundTex2);
	m_background2.setPosition(sf::Vector2f(-20, -32));

}

void Scene_Zelda::loadLevel(const std::string& filename)
{
	m_game->window().clear(sf::Color::Transparent);
	baseFilter = m_game->window().capture();
	filterImage = baseFilter;

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
		if (line == "Tile")
		{
			std::string animation;
			int roomX;
			int roomY;
			int tileX;
			int tileY;
			int sizeX;
			int sizeY;
			bool blockMove;
			bool blockVis;

			auto t = m_entityManager.addEntity("tile");
			iss >> animation >> roomX >> roomY >> tileX >> tileY >> sizeX >> sizeY >> blockMove >> blockVis;
			t->addComponent<CAnimation>(m_game->assets().getAnimation(animation), true);
			t->addComponent<CTransform>(getPosition(roomX, roomY, tileX, tileY));
			t->addComponent<CBoundingBox>(Vec2(sizeX, sizeY), blockMove, blockVis);
			if (animation == "GravityWell")
			{
				t->addComponent<CRange>(Vec2(sizeX, sizeY), 5);
			}

		}
		if (line == "MovingTile")
		{
			std::string animation;
			int roomX;
			int roomY;
			int tileX;
			int tileY;
			int sizeX;
			int sizeY;
			float speed;
			int numPatrol;
			std::vector<Vec2> patrolCoords;

			auto t = m_entityManager.addEntity("tile");
			iss >> animation >> roomX >> roomY >> tileX >> tileY >> sizeX >> sizeY >> speed >> numPatrol;
			t->addComponent<CAnimation>(m_game->assets().getAnimation(animation), true);
			t->addComponent<CTransform>(getPosition(roomX, roomY, tileX, tileY));
			t->addComponent<CBoundingBox>(Vec2(sizeX, sizeY), 0, 0);
			if (animation == "GravityWell")
			{
				t->addComponent<CRange>(Vec2(sizeX, sizeY), 5);
			}
			for (int i = 0; i < numPatrol; i++)
			{
				int x, y;
				iss >> x >> y;
				patrolCoords.push_back(Vec2(x, y));
			}
			for (size_t i = 0; i < patrolCoords.size(); i++)
			{
				patrolCoords[i] = getPosition(roomX, roomY, patrolCoords[i].x, patrolCoords[i].y);
			}
			t->addComponent<CPatrol>(patrolCoords, speed);
			t->getComponent<CPatrol>().currentPosition = 0;

		}
		if (line == "Dec")
		{
			std::string animation;
			int roomX, roomY, gridX, gridY;
			auto d = m_entityManager.addEntity("decoration");
			iss >> animation >> roomX >> roomY >> gridX >> gridY;
			d->addComponent<CAnimation>(m_game->assets().getAnimation(animation), true);
			d->addComponent<CTransform>(getPosition(roomX, roomY, gridX, gridY));
		}
		if (line == "Land")
		{
			std::string animation;
			int roomX, roomY, gridX, gridY;
			int sizeX;
			int sizeY;
			bool blockMove;
			bool blockVis;
			auto d = m_entityManager.addEntity("Land");
			iss >> animation >> roomX >> roomY >> gridX >> gridY >> sizeX >> sizeY >> blockMove >> blockVis;
			d->addComponent<CAnimation>(m_game->assets().getAnimation(animation), true);
			d->addComponent<CBoundingBox>(Vec2(sizeX, sizeY), blockMove, blockVis);
			d->addComponent<CTransform>(getPosition(roomX, roomY, gridX, gridY));
		}
		if (line == "Water")
		{
			std::string animation;
			int roomX, roomY, gridX, gridY;
			int sizeX;
			int sizeY;
			bool blockMove;
			bool blockVis;
			auto d = m_entityManager.addEntity("Water");
			iss >> animation >> roomX >> roomY >> gridX >> gridY >> sizeX >> sizeY >> blockMove >> blockVis;
			d->addComponent<CAnimation>(m_game->assets().getAnimation(animation), true);
			d->addComponent<CBoundingBox>(Vec2(sizeX, sizeY), blockMove, blockVis);
			d->addComponent<CTransform>(getPosition(roomX, roomY, gridX, gridY));
		}
		if (line == "Player")
		{
			float spawnX, spawnY, characterWidth, characterHeight, maxHealth;
			float speed;

			iss >> spawnX >> spawnY >> characterWidth >> characterHeight >> speed >> maxHealth;

			m_playerConfig.X = spawnX;
			m_playerConfig.Y = spawnY;
			m_playerConfig.CX = characterWidth;
			m_playerConfig.CY = characterHeight;
			m_playerConfig.SPEED = speed;
			m_playerConfig.HEALTH = maxHealth;
		}
		if (line == "NPC")
		{
			std::string animation, behavior;
			int roomX, roomY, tileX, tileY, maxHealth, damage;
			int sizeX;
			int sizeY;
			bool blockMove, blockVis;
			float speed;
			std::vector<Vec2> patrolCoords;
			iss >> animation >> roomX >> roomY >> tileX >> tileY >> sizeX >> sizeY >> blockMove >> blockVis >> maxHealth >> damage >> behavior >> speed;
			auto n = m_entityManager.addEntity("NPC");
			n->addComponent<CAnimation>(m_game->assets().getAnimation(animation), true);
			n->addComponent<CTransform>(getPosition(roomX, roomY, tileX, tileY));
			n->addComponent<CBoundingBox>(Vec2(sizeX, sizeY), blockMove, blockVis);
			if (behavior != "Mimic")
			{
				n->addComponent<CHealth>(maxHealth, maxHealth);
			}
			n->addComponent<CDamage>(damage);
			n->addComponent<CBuffs>();
			if (n->getComponent<CHealth>().max >= 15) {
				if (bossExists) {
					if (m_Boss->getComponent<CHealth>().max < n->getComponent<CHealth>().max) {
						m_Boss->removeComponent<CBoss>();
						n->addComponent<CBoss>();
						if (filename == "campaignLevels/level3.txt")
						{
							MakeBoss(n, animation, true, true, false);
							n->getComponent<CBoss>().healthBars = 2;

						}
						else
						{
							MakeBoss(n, animation, true, false, true);
						}
					}
				}
				if (!bossExists) {
					n->addComponent<CBoss>();
					if (filename == "campaignLevels/level3.txt")
					{
						MakeBoss(n, animation, true, true, false);
						n->getComponent<CBoss>().healthBars = 2;
					}
					else
					{
						MakeBoss(n, animation, true, false, true);
					}
				}
			}

			if (behavior == "Range")
			{
				n->addComponent<CRange>(m_game->assets().getAnimation(animation).getSize(), speed * 2);
				iss >> behavior >> speed;
			}
			if (behavior == "Patrol")
			{
				int numPatrol;
				iss >> numPatrol;
				for (int i = 0; i < numPatrol; i++)
				{
					int x, y;
					iss >> x >> y;
					patrolCoords.push_back(Vec2(x, y));
				}

				for (size_t i = 0; i < patrolCoords.size(); i++)
				{
					patrolCoords[i] = getPosition(roomX, roomY, patrolCoords[i].x, patrolCoords[i].y);
				}
				n->addComponent<CPatrol>(patrolCoords, speed);
				n->getComponent<CPatrol>().currentPosition = 0;
			}
			else if (behavior == "Follow")
			{
				n->addComponent<CFollowPlayer>(getPosition(roomX, roomY, tileX, tileY), speed);

			}
			else if (behavior == "Mimic")
			{
				n->addComponent<CMimic>(m_game->assets().getAnimation(animation).getSize(), 5, maxHealth, getPosition(roomX, roomY, tileX, tileY), speed);
			}

		}
		if (line == "Dark")
		{
			m_game->window().clear(sf::Color::Black);
			baseFilter = m_game->window().capture();
			m_playerConfig.hasTorch = true;
		}
		if (line == "LightSource")
		{
			std::string animation;
			int roomX, roomY, tileX, tileY;
			int sizeX;
			int sizeY;
			bool blockMove;
			iss >> animation >> roomX >> roomY >> tileX >> tileY >> sizeX >> sizeY >> blockMove;
			auto n = m_entityManager.addEntity("tile");
			n->addComponent<CAnimation>(m_game->assets().getAnimation(animation), true);
			n->addComponent<CTransform>(getPosition(roomX, roomY, tileX, tileY));
			n->addComponent<CBoundingBox>(Vec2(sizeX, sizeY), blockMove, false);
			n->addComponent<CLight>(3 * m_game->assets().getAnimation(animation).getSize().x / 2);
			auto size = n->getComponent<CLight>().light;
			auto pos = n->getComponent<CTransform>().pos;
			for (auto x = (pos.x - size); x < (pos.x + size); x++)
			{
				for (auto y = (pos.y - size); y < (pos.y + size); y++)
				{
					lightPixels.push_back(Vec2(x, y));
				}
			}
		}
	}

	m_game->loopSound("MusicLevel");

	// HERE IS MY TEST CODE IF THIS IS PUSHED TO GIT DELETE BETWEEN HERE AND NEXT COMMENT
	/*
	auto n = m_entityManager.addEntity("Test");
	n->addComponent<CAnimation>(m_game->assets().getAnimation("RunRight"), true);
	n->addComponent<CTransform>(Vec2(640, 450));
	n->addComponent<CBoundingBox>(m_game->assets().getAnimation("RunRight").getSize(), 0, 1);
	n->addComponent<CHealth>(20, 20);
	n->addComponent<CDamage>(0);
	n->addComponent<CBuffs>();
	*/

	// HERE IS MY TEST CODE IF THIS IS PUSHED TO GIT DELETE BETWEEN HERE AND LAST COMMENT
	spawnPlayer();
}

Vec2 Scene_Zelda::getPosition(int rx, int ry, int tx, int ty) const
{
	return Vec2((rx * 1280) + ((tx + 0.5) * 64), (ry * 768) + ((ty + 0.5) * 64));
}

void Scene_Zelda::spawnPlayer()
{
	playerLives -= 1;
	if (playerLives < 1) {
		m_game->assets().getSound("MusicLevel").stop();
		m_game->changeScene("GameOver", std::make_shared<Scene_Game_Over>(m_game));
	}
	m_player = m_entityManager.addEntity("player");
	m_player->addComponent<CTransform>(Vec2(m_playerConfig.X, m_playerConfig.Y));
	m_player->addComponent<CAnimation>(m_game->assets().getAnimation("StandDown"), true);
	m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY), true, false);
	m_player->addComponent<CHealth>(m_playerConfig.HEALTH * difficulty, m_playerConfig.HEALTH * difficulty);
	m_player->addComponent<CInput>();
	m_player->addComponent<CInventory>();
	m_player->addComponent<CBuffs>();
	m_player->addComponent<CState>();
	m_player->getComponent<CState>().state = "StandingDown";
	if (m_playerConfig.hasTorch)
	{
		m_player->addComponent<CLight>(3 * m_playerConfig.CX);
	}
	for (auto n : m_entityManager.getEntities("NPC"))
	{
		if (n->hasComponent<CFollowPlayer>()) {
			n->getComponent<CFollowPlayer>().hasSeenPlayer = false;

		}
	}

}

void Scene_Zelda::spawnSword(std::shared_ptr<Entity> entity)
{
	// Implement the spawning of the sword
	auto swordCheck = m_entityManager.getEntities("Sword");
	if (m_player->getComponent<CInput>().attack == true && swordCheck.size() != 1) {
		m_game->playSound("Slash");

		auto sword = m_entityManager.addEntity("Sword");
		const auto& playerTransform = entity->getComponent<CTransform>();
		sword->addComponent<CTransform>(Vec2(playerTransform.pos.x, playerTransform.pos.y));
		sword->addComponent<CLifeSpan>(10, m_currentFrame);


		// Here we add the sword component multiplied by the difficulty to make sure
		//it's always at the same level of difficulty as the player currently is
		// Because it's an int if it gets reduced below one it gets set to one.

		if (entity->getComponent<CBuffs>().attackBuffed)
		{
			sword->addComponent<CDamage>((2 * difficulty) + entity->getComponent<CBuffs>().attackBuffStrength);
		}
		else
		{
			sword->addComponent<CDamage>(2 * difficulty);
		}
		if (sword->getComponent<CDamage>().damage == 0) {
			sword->getComponent<CDamage>().damage = 1;
		}
		if (entity->getComponent<CBuffs>().attacksPoison)
		{
			auto buffs = entity->getComponent<CBuffs>();
			sword->addComponent<CBuffs>(true, buffs.poisonCharges, buffs.poisonDamage, buffs.poisonRate, buffs.poisonDuration);
		}

		if (entity->getComponent<CTransform>().facing.x != 0) {
			if (entity->getComponent<CTransform>().facing.x > 0) {
				sword->addComponent<CAnimation>(m_game->assets().getAnimation("SwordRight"), true);
				sword->addComponent<CBoundingBox>(m_game->assets().getAnimation("SwordRight").getSize() + Vec2(10, 10));
				sword->addComponent<CTransform>(Vec2(playerTransform.pos.x + 47, playerTransform.pos.y));
			}
			else {
				sword->addComponent<CAnimation>(m_game->assets().getAnimation("SwordRight"), true);
				sword->addComponent<CBoundingBox>(m_game->assets().getAnimation("SwordRight").getSize() + Vec2(10, 10));
				sword->addComponent<CTransform>(Vec2(playerTransform.pos.x - 47, playerTransform.pos.y));
				sword->getComponent<CTransform>().scale.x = -1;
			}
		}
		if (entity->getComponent<CTransform>().facing.y != 0) {
			if (entity->getComponent<CTransform>().facing.y > 0) {
				sword->addComponent<CAnimation>(m_game->assets().getAnimation("SwordUp"), true);
				sword->addComponent<CBoundingBox>(m_game->assets().getAnimation("SwordUp").getSize() + Vec2(10, 10));
				sword->addComponent<CTransform>(Vec2(playerTransform.pos.x, playerTransform.pos.y - 47));
			}
			else {
				sword->addComponent<CAnimation>(m_game->assets().getAnimation("SwordDown"), true);
				sword->addComponent<CBoundingBox>(m_game->assets().getAnimation("SwordDown").getSize() + Vec2(10, 10));
				sword->addComponent<CTransform>(Vec2(playerTransform.pos.x, playerTransform.pos.y + 47));
			}
		}


	}


}

void Scene_Zelda::spawnArrow(std::shared_ptr<Entity> source, std::shared_ptr<Entity> target)
{
	auto arrowType = "nArrow";
	if (!source->hasComponent<CAttackCooldown>())
	{
		auto arrow = m_entityManager.addEntity(arrowType);
		auto sourceTransform = source->getComponent<CTransform>();
		auto life = source->getComponent<CRange>().range;
		arrow->addComponent<CTransform>(sourceTransform.pos);
		arrow->addComponent<CAnimation>(m_game->assets().getAnimation("Arrow"), true);
		arrow->addComponent<CBoundingBox>(Vec2(m_game->assets().getAnimation("Arrow").getSize().y, m_game->assets().getAnimation("Arrow").getSize().y));
		arrow->addComponent<CDamage>(1);
		if (source->hasComponent<CBoss>()) {
			if (source->getComponent<CBoss>().gravityArrows == true) {
				arrow->addComponent<CSpecialArrow>();
				arrow->getComponent<CSpecialArrow>().spawnGravity = true;
			}
			if (source->getComponent<CBoss>().moveTileArrows == true) {
				arrow->addComponent<CSpecialArrow>();
				arrow->getComponent<CSpecialArrow>().moveTile = true;
			}
			if (source->getComponent<CBoss>().poisonArrows == true) {
				arrow->getComponent<CBuffs>().attacksPoison = true;
				arrow->getComponent<CBuffs>().poisonCharges = 1;
				arrow->getComponent<CBuffs>().poisonDamage = 1;
				arrow->getComponent<CBuffs>().poisonDuration = 3;

			}
		}

		auto nPos = arrow->getComponent<CTransform>().pos;
		if (abs(nPos.x - target->getComponent<CTransform>().pos.x) > 5
			|| abs(nPos.y - target->getComponent<CTransform>().pos.y) > 5)
		{
			float xDist = nPos.x - target->getComponent<CTransform>().pos.x;
			float yDist = nPos.y - target->getComponent<CTransform>().pos.y;
			float percentageX = xDist / (abs(xDist) + abs(yDist));
			float percentageY = yDist / (abs(xDist) + abs(yDist));
			arrow->getComponent<CTransform>().velocity.x = -percentageX * 10;
			arrow->getComponent<CTransform>().velocity.y = -percentageY * 10;

			float ang = std::atan(yDist / xDist) * 180 / 3.14159265358979;
			if (xDist >= 0)
			{
				ang += 180;
			}
			arrow->getComponent<CTransform>().angle = ang;
		}
		source->addComponent<CAttackCooldown>(60, m_currentFrame);
	}
}

void Scene_Zelda::spawnArrowP(std::shared_ptr<Entity> source, Vec2 target)
{
	sf::View view = m_game->window().getView();
	auto upperx = m_game->window().getView().getCenter().x - m_game->window().getSize().x / 2;
	auto uppery = m_game->window().getView().getCenter().y - m_game->window().getSize().y / 2;
	target.x += upperx;
	target.y += uppery;

	auto arrowType = "pArrow";
	if (source->getComponent<CInventory>().arrows > 0)
	{
		if (!source->hasComponent<CAttackCooldown>())
		{
			auto arrow = m_entityManager.addEntity(arrowType);
			auto sourceTransform = source->getComponent<CTransform>();
			arrow->addComponent<CTransform>(sourceTransform.pos);
			arrow->addComponent<CAnimation>(m_game->assets().getAnimation("Arrow"), true);
			arrow->addComponent<CBoundingBox>(Vec2(m_game->assets().getAnimation("Arrow").getSize().y, m_game->assets().getAnimation("Arrow").getSize().y));
			arrow->addComponent<CDamage>(1);

			auto nPos = arrow->getComponent<CTransform>().pos;
			if (abs(nPos.x - target.x) > 5
				|| abs(nPos.y - target.y) > 5)
			{
				float xDist = nPos.x - target.x;
				float yDist = nPos.y - target.y;
				float percentageX = xDist / (abs(xDist) + abs(yDist));
				float percentageY = yDist / (abs(xDist) + abs(yDist));
				arrow->getComponent<CTransform>().velocity.x = -percentageX * 10;
				arrow->getComponent<CTransform>().velocity.y = -percentageY * 10;

				float ang = std::atan(yDist / xDist) * 180 / 3.14159265358979;
				if (xDist >= 0)
				{
					ang += 180;
				}
				arrow->getComponent<CTransform>().angle = ang;
				m_player->getComponent<CInventory>().arrows -= 1;
			}
			source->addComponent<CAttackCooldown>(30, m_currentFrame);
		}
	}
}

void Scene_Zelda::update()
{
	if (!m_paused) {
		m_entityManager.update();

		sAI();
		sMovement();
		sStatus();
		sCollision();
		sAnimation();
		sCamera();

		m_currentFrame++;
	}

}

void Scene_Zelda::sMovement()
{
	int currentSpeed;
	if (m_player->getComponent<CBuffs>().speedBuffed)
	{
		currentSpeed = m_playerConfig.SPEED + m_player->getComponent<CBuffs>().speedBuffStrength;
	}
	else
	{
		currentSpeed = m_playerConfig.SPEED;
	}
	// boolean math to determine speed
	// if opposite directions held, 1-1 = 0 * SPEED = 0. Same if neither direction is held (0-0 = 0). 
	// If one direction is held, it is 1*SPEED or -1*SPEED, to determine direction.
	if (m_player->hasComponent<CInput>())
	{
		m_player->getComponent<CTransform>().velocity.x = (m_player->getComponent<CInput>().right - m_player->getComponent<CInput>().left) * currentSpeed;
		if (m_player->getComponent<CTransform>().velocity.x == 0) {
			m_player->getComponent<CTransform>().velocity.y = (m_player->getComponent<CInput>().down - m_player->getComponent<CInput>().up) * currentSpeed;

			if (m_player->getComponent<CTransform>().velocity.y > 0) {
				m_player->getComponent<CState>().state = "MovingDown";
			}
			if (m_player->getComponent<CTransform>().velocity.y < 0) {
				m_player->getComponent<CState>().state = "MovingUp";
			}
		}
		else {
			m_player->getComponent<CTransform>().velocity.y = 0;
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

void Scene_Zelda::sDoAction(const Action& action)
{
	// STUDENT TODO:
	//
	// Implement all actions described for the game here
	// Only the setting of the player's input component variables should be set here
	// Do minimal logic in this function
	if (assigningHotkey == true) {
		sf::Event event;
		while (m_game->window().waitEvent(event) && assigningHotkey == true)
		{
			if (event.type == sf::Event::KeyPressed)
			{
				bool alreadyUsed = false;
				for (auto Key : m_Controls.buttonMap) {
					if (Key.second == event.key.code) {
						m_game->window().clear(sf::Color(255, 0, 0, 255));
						alreadyUsed = true;
					}
				}
				if (!alreadyUsed) {
					const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";
					registerAction(m_Controls.buttonMap[SelectedMenuOption], "NONE");
					m_Controls.updateHotkey(SelectedMenuOption, event.key.code);
					registerAction(m_Controls.buttonMap[SelectedMenuOption], SelectedMenuOption);
					assigningHotkey = false;
					hotkeyMenu = true;
					m_selectedMenuOption = 0;
				}
			}
		}
	}

	if (action.type() == "START")
	{
		// These are the default scene actions if the game is not paused
		if (!m_paused) {

			if (action.name() == "PAUSE") { setPaused(!m_paused); }
			else if (action.name() == "QUIT") { onEnd(); }
			else if (action.name() == "TOGGLE_FOLLOW") { m_follow = !m_follow; }
			else if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
			else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
			else if (action.name() == "MOVE_UP") { m_player->getComponent<CInput>().up = true; }
			else if (action.name() == "MOVE_LEFT") { m_player->getComponent<CInput>().left = true; }
			else if (action.name() == "MOVE_DOWN") { m_player->getComponent<CInput>().down = true; }
			else if (action.name() == "MOVE_RIGHT") { m_player->getComponent<CInput>().right = true; }
			else if (action.name() == "ATTACK") {
				m_player->getComponent<CInput>().attack = true;
				spawnSword(m_player);
			}
			else if (action.name() == "FIRE_ARROW") {
				spawnArrowP(m_player, Vec2((float)sf::Mouse::getPosition(m_game->window()).x, (float)sf::Mouse::getPosition(m_game->window()).y));
			}
		}
		/*
		When paused this block will begin, this same trigger is what causes the
		menu to begin rendering instead of the level. While paused it will use
		the same keybinds as the main scene while letting you change the settings
		for the game.

		Each action is read as normal but then dependent on the current menu
		bool in use the game will do the proper action for the current selection.
		*/
		else {

			if (action.name() == "PAUSE") {
				setPaused(!m_paused);
				navigateMenu("");
			}

			else if (action.name() == "MOVE_UP")
			{

				if (musicVolume && musicLevel < 100) {
					musicLevel += 1;
					setSoundLevel(musicLevel, "Music");
				}
				if (soundEffectVolume && soundEffectLevel < 100) {
					soundEffectLevel += 1;
					setSoundLevel(soundEffectLevel, "SoundEffect");
				}
				if (m_selectedMenuOption > 0) { m_selectedMenuOption--; }
				else { m_selectedMenuOption = m_menuStrings.size() - 1; }
			}
			else if (action.name() == "MOVE_DOWN")
			{
				m_selectedMenuOption = (m_selectedMenuOption + 1) % m_menuStrings.size();
				if (musicVolume && musicLevel > 0) {
					musicLevel -= 1;
					setSoundLevel(musicLevel, "Music");
				}
				if (soundEffectVolume && soundEffectLevel > 0) {
					soundEffectLevel -= 1;
					setSoundLevel(soundEffectLevel, "SoundEffect");
				}
			}
			else if (action.name() == "MOVE_RIGHT")
			{
				if (m_menuStrings[m_selectedMenuOption] == "Change Hotkey") {
					navigateMenu("hotkeyMenu");
				}
				else if (m_menuStrings[m_selectedMenuOption] == "Music Volume") {
					navigateMenu("musicVolume");
				}
				else if (m_menuStrings[m_selectedMenuOption] == "Sound Effects Volume") {
					navigateMenu("soundEffectVolume");
				}
				else if (m_menuStrings[m_selectedMenuOption] == "Game Difficulty") {
					navigateMenu("gameDifficulty");
				}
				else if (m_menuStrings[m_selectedMenuOption] == "Inventory") {
					navigateMenu("inventory");
				}
				else if (m_menuStrings[m_selectedMenuOption] == ("Health Potions x" + std::to_string(m_player->getComponent<CInventory>().healthPotions)))
				{
					if (m_player->getComponent<CInventory>().healthPotions > 0) {
						m_player->getComponent<CInventory>().healthPotions -= 1;
						m_player->getComponent<CHealth>().current = m_playerConfig.HEALTH * difficulty;
					}
				}

				else if (m_menuStrings[m_selectedMenuOption] == ("Strength Potions x" + std::to_string(m_player->getComponent<CInventory>().strengthPotions)))
				{
					if (m_player->getComponent<CInventory>().strengthPotions > 0) {
						m_player->getComponent<CInventory>().strengthPotions -= 1;
						m_player->getComponent<CBuffs>().attackBuffed = true;
						m_player->getComponent<CBuffs>().attackBuffStrength = 2;
						m_player->getComponent<CBuffs>().attackBuffDuration = 1200;
					}
				}

				else if (m_menuStrings[m_selectedMenuOption] == ("Speed Potions x" + std::to_string(m_player->getComponent<CInventory>().speedPotions)))
				{
					if (m_player->getComponent<CInventory>().speedPotions > 0) {
						m_player->getComponent<CInventory>().speedPotions -= 1;
						m_player->getComponent<CBuffs>().speedBuffed = true;
						m_player->getComponent<CBuffs>().speedBuffStrength = 2;
						m_player->getComponent<CBuffs>().speedBuffDuration = 1200;
					}
				}

				else if (m_menuStrings[m_selectedMenuOption] == ("Poison Flasks x" + std::to_string(m_player->getComponent<CInventory>().poisonFlasks)))
				{
					if (m_player->getComponent<CInventory>().poisonFlasks > 0) {
						m_player->getComponent<CInventory>().poisonFlasks -= 1;
						m_player->getComponent<CBuffs>().attacksPoison = true;
						m_player->getComponent<CBuffs>().poisonCharges = 3;
						m_player->getComponent<CBuffs>().poisonDamage = 1;
						m_player->getComponent<CBuffs>().poisonRate = 60;
						m_player->getComponent<CBuffs>().poisonDuration = 3;
					}
				}

				else if (hotkeyMenu == true) {

					SelectedMenuOption = m_menuStrings[m_selectedMenuOption];
					navigateMenu("assigningHotkey");
				}
				else if (m_menuStrings[m_selectedMenuOption] == "\nEasy") {
					changeDifficulty(m_menuStrings[m_selectedMenuOption]);
				}
				else if (m_menuStrings[m_selectedMenuOption] == "\nNormal") {
					changeDifficulty(m_menuStrings[m_selectedMenuOption]);
				}
				else if (m_menuStrings[m_selectedMenuOption] == "\nHard") {
					changeDifficulty(m_menuStrings[m_selectedMenuOption]);
				}

			}
			else if (action.name() == "MOVE_LEFT") {
				if (hotkeyMenu == true) {
					navigateMenu("");
				}
				else if (assigningHotkey == true) {
					navigateMenu("hotKeyMenu");
				}
				else if (musicVolume == true) {
					navigateMenu("");
				}
				else if (soundEffectVolume == true) {
					navigateMenu("");
				}
				else if (gameDifficulty == true) {
					navigateMenu("");
				}
				else if (inventory == true) {
					navigateMenu("");
				}
				else {
					setPaused(!m_paused);
					navigateMenu("");
				}
			}
			else if (action.name() == "QUIT")
			{
				onEnd();
			}
		}

	}
	else if (action.type() == "END")
	{
		if (action.name() == "MOVE_UP") { m_player->getComponent<CInput>().up = false; }
		else if (action.name() == "MOVE_LEFT") { m_player->getComponent<CInput>().left = false; }
		else if (action.name() == "MOVE_DOWN") { m_player->getComponent<CInput>().down = false; }
		else if (action.name() == "MOVE_RIGHT") { m_player->getComponent<CInput>().right = false; }
		else if (action.name() == "ATTACK") { m_player->getComponent<CInput>().attack = false; }
	}
}


void Scene_Zelda::sAI()
{
	AStar pathFinder;
	pathFinder.Init(m_entityManager);

	for (auto n : m_entityManager.getEntities())
	{
		if (n->hasComponent<CPatrol>() && n->getComponent<CPatrol>().positions.size() != 0) {
			auto nPos = n->getComponent<CTransform>().pos;
			auto nPatrolPos = n->getComponent<CPatrol>().positions[n->getComponent<CPatrol>().currentPosition];
			if (n->getComponent<CTransform>().velocity.x == 0 && n->getComponent<CTransform>().velocity.y == 0) {
				float xDist = nPos.x - n->getComponent<CPatrol>().positions[n->getComponent<CPatrol>().currentPosition].x;
				float yDist = nPos.y - n->getComponent<CPatrol>().positions[n->getComponent<CPatrol>().currentPosition].y;
				float percentageX = xDist / (xDist + yDist);
				float percentageY = yDist / (xDist + yDist);
				if (xDist > 0) {
					percentageX = -percentageX;
				}
				if (yDist > 0) {
					percentageY = -percentageY;
				}
				n->getComponent<CTransform>().velocity.x = percentageX * n->getComponent<CPatrol>().speed;
				n->getComponent<CTransform>().velocity.y = percentageY * n->getComponent<CPatrol>().speed;
			}

			if (abs(n->getComponent<CPatrol>().positions[n->getComponent<CPatrol>().currentPosition].x - n->getComponent<CTransform>().pos.x) < 5) {
				if (abs(n->getComponent<CPatrol>().positions[n->getComponent<CPatrol>().currentPosition].y - n->getComponent<CTransform>().pos.y) < 5) {
					n->getComponent<CPatrol>().currentPosition += 1;
					if (n->getComponent<CPatrol>().currentPosition == n->getComponent<CPatrol>().positions.size()) {
						n->getComponent<CPatrol>().currentPosition = 0;
					}

					float xDist = nPos.x - n->getComponent<CPatrol>().positions[n->getComponent<CPatrol>().currentPosition].x;
					float yDist = nPos.y - n->getComponent<CPatrol>().positions[n->getComponent<CPatrol>().currentPosition].y;
					float percentageX = xDist / (abs(xDist) + abs(yDist));
					float percentageY = yDist / (abs(xDist) + abs(yDist));

					n->getComponent<CTransform>().velocity.x = -percentageX * n->getComponent<CPatrol>().speed;
					n->getComponent<CTransform>().velocity.y = -percentageY * n->getComponent<CPatrol>().speed;
				}
			}
		}
		if (n->hasComponent<CFollowPlayer>())
		{
			bool blocked = false;
			for (auto t : m_entityManager.getEntities("tile"))
			{
				if (t->getComponent<CBoundingBox>().blockVision &&
					Physics::EntityIntersect(n->getComponent<CTransform>().pos, m_player->getComponent<CTransform>().pos, t))
				{
					blocked = true;
				}
			}
			for (auto t : m_entityManager.getEntities("Land"))
			{
				if (t->getComponent<CBoundingBox>().blockVision &&
					Physics::EntityIntersect(n->getComponent<CTransform>().pos, m_player->getComponent<CTransform>().pos, t))
				{
					blocked = true;
				}
			}
			for (auto t : m_entityManager.getEntities("Water"))
			{
				if (t->getComponent<CBoundingBox>().blockVision &&
					Physics::EntityIntersect(n->getComponent<CTransform>().pos, m_player->getComponent<CTransform>().pos, t))
				{
					blocked = true;
				}
			}
			auto nPos = n->getComponent<CTransform>().pos;
			if (!blocked)
			{
				m_player->getComponent<CTransform>().prevPos = m_player->getComponent<CTransform>().pos;
				n->getComponent<CFollowPlayer>().hasSeenPlayer = true;
				n->getComponent<CFollowPlayer>().pathCalculated = false;
				if (abs(nPos.x - m_player->getComponent<CTransform>().pos.x) > 5
					|| abs(nPos.y - m_player->getComponent<CTransform>().pos.y) > 5)
				{
					float xDist = nPos.x - m_player->getComponent<CTransform>().pos.x;
					float yDist = nPos.y - m_player->getComponent<CTransform>().pos.y;
					float percentageX = xDist / (abs(xDist) + abs(yDist));
					float percentageY = yDist / (abs(xDist) + abs(yDist));
					n->getComponent<CTransform>().velocity.x = -percentageX * n->getComponent<CFollowPlayer>().speed;
					n->getComponent<CTransform>().velocity.y = -percentageY * n->getComponent<CFollowPlayer>().speed;
				}
				else
				{
					n->getComponent<CTransform>().velocity.x = 0;
					n->getComponent<CTransform>().velocity.y = 0;
				}
			}
			else
			{
				if (!n->getComponent<CFollowPlayer>().hasSeenPlayer || (abs(nPos.x - m_player->getComponent<CTransform>().pos.x) > 500 
					|| abs(nPos.y - m_player->getComponent<CTransform>().pos.y) > 500)) {
					if (abs(nPos.x - n->getComponent<CFollowPlayer>().home.x) > 5
						|| abs(nPos.y - n->getComponent<CFollowPlayer>().home.y) > 5)
					{
						float xDist = nPos.x - n->getComponent<CFollowPlayer>().home.x;
						float yDist = nPos.y - n->getComponent<CFollowPlayer>().home.y;
						float percentageX = xDist / (abs(xDist) + abs(yDist));
						float percentageY = yDist / (abs(xDist) + abs(yDist));
						n->getComponent<CTransform>().velocity.x = -percentageX * n->getComponent<CFollowPlayer>().speed;
						n->getComponent<CTransform>().velocity.y = -percentageY * n->getComponent<CFollowPlayer>().speed;
						n->getComponent<CFollowPlayer>().pathCalculated = false;
						if (xDist < 128 && yDist < 128) {

							n->getComponent<CFollowPlayer>().hasSeenPlayer = false;
						}

					}
					else
					{
						n->getComponent<CTransform>().velocity.x = 0;
						n->getComponent<CTransform>().velocity.y = 0;
					}
				}
				else if (n->getComponent<CFollowPlayer>().hasSeenPlayer)  {
		if (abs(m_player->getComponent<CTransform>().prevPos.x - m_player->getComponent<CTransform>().pos.x) < 5
			|| abs(m_player->getComponent<CTransform>().prevPos.y - m_player->getComponent<CTransform>().pos.y) < 5) {
			if (!n->getComponent<CFollowPlayer>().pathCalculated) {
				n->getComponent<CFollowPlayer>().positions = pathFinder.findPath(n, m_player);
				n->getComponent<CFollowPlayer>().currentPosition = 0;
				n->getComponent<CFollowPlayer>().pathCalculated = true;
			}
			float xDist = nPos.x - n->getComponent<CFollowPlayer>().positions[(n->getComponent<CFollowPlayer>().positions.size() - n->getComponent<CFollowPlayer>().currentPosition - 1) % n->getComponent<CFollowPlayer>().positions.size()].x;
			float yDist = nPos.y - n->getComponent<CFollowPlayer>().positions[(n->getComponent<CFollowPlayer>().positions.size() - n->getComponent<CFollowPlayer>().currentPosition - 1) % n->getComponent<CFollowPlayer>().positions.size()].y;
			if (abs(xDist) < 2 && abs(yDist) < 2) {
				n->getComponent<CFollowPlayer>().currentPosition += 1;
				if (n->getComponent<CFollowPlayer>().currentPosition  == n->getComponent<CFollowPlayer>().positions.size()) {
					n->getComponent<CFollowPlayer>().currentPosition = 0;
					n->getComponent<CFollowPlayer>().pathCalculated = false;
				}
				xDist = nPos.x - n->getComponent<CFollowPlayer>().positions[(n->getComponent<CFollowPlayer>().positions.size() - n->getComponent<CFollowPlayer>().currentPosition - 1) % n->getComponent<CFollowPlayer>().positions.size()].x;
				yDist = nPos.y - n->getComponent<CFollowPlayer>().positions[(n->getComponent<CFollowPlayer>().positions.size() - n->getComponent<CFollowPlayer>().currentPosition - 1) % n->getComponent<CFollowPlayer>().positions.size()].y;
			}

			
			float percentageX = xDist / (abs(xDist) + abs(yDist));
			float percentageY = yDist / (abs(xDist) + abs(yDist));
			n->getComponent<CTransform>().velocity.x = -percentageX * n->getComponent<CFollowPlayer>().speed;
			n->getComponent<CTransform>().velocity.y = -percentageY * n->getComponent<CFollowPlayer>().speed;
		}
		else //if (abs(nPos.x - m_player->getComponent<CTransform>().pos.x) > 5
						//|| abs(nPos.y - m_player->getComponent<CTransform>().pos.y) > 5)
					{

						n->getComponent<CFollowPlayer>().positions = pathFinder.findPath(n, m_player); 
						n->getComponent<CFollowPlayer>().pathCalculated = true;
							n->getComponent<CFollowPlayer>().currentPosition = 0;
							float xDist = nPos.x - n->getComponent<CFollowPlayer>().positions[n->getComponent<CFollowPlayer>().positions.size() - n->getComponent<CFollowPlayer>().currentPosition - 1].x;
							float yDist = nPos.y - n->getComponent<CFollowPlayer>().positions[n->getComponent<CFollowPlayer>().positions.size() - n->getComponent<CFollowPlayer>().currentPosition - 1].y;

							float percentageX = xDist / (abs(xDist) + abs(yDist));
						float percentageY = yDist / (abs(xDist) + abs(yDist));
						n->getComponent<CTransform>().velocity.x = -percentageX * n->getComponent<CFollowPlayer>().speed;
						n->getComponent<CTransform>().velocity.y = -percentageY * n->getComponent<CFollowPlayer>().speed;
						m_player->getComponent<CTransform>().prevPos = m_player->getComponent<CTransform>().pos;

					}

					//else
					//{
					//	n->getComponent<CTransform>().velocity.x = 0;
					//	n->getComponent<CTransform>().velocity.y = 0;
					//}
				}
			}
		}

		if (n->hasComponent<CMimic>())
		{
			if (Physics::IsInAggroRange(m_player->getComponent<CTransform>().pos, n))
			{

				n->addComponent<CFollowPlayer>(n->getComponent<CMimic>().home, n->getComponent<CMimic>().speed);
				n->addComponent<CHealth>(n->getComponent<CMimic>().hp, n->getComponent<CMimic>().hp);
				n->removeComponent<CMimic>();
			}
		}
	}
}

void Scene_Zelda::sStatus()
{
	for (auto e : m_entityManager.getEntities())
	{
		if (e->hasComponent<CLifeSpan>())
		{
			if (e->getComponent<CLifeSpan>().frameCreated + e->getComponent<CLifeSpan>().lifespan < m_currentFrame)
			{
				e->destroy();
			}
		}
		if (e->hasComponent<CInvincibility>())
		{
			if (e->getComponent<CInvincibility>().iframes <= 0)
			{
				e->removeComponent<CInvincibility>();
			}
			else {
				e->getComponent<CInvincibility>().iframes--;
			}
		}
		if (e->hasComponent<CAttackCooldown>())
		{
			if (e->getComponent<CAttackCooldown>().frameCreated + e->getComponent<CAttackCooldown>().cooldown < m_currentFrame)
			{
				e->removeComponent<CAttackCooldown>();
			}
		}
		if (e->hasComponent<CPoisoned>())
		{
			if (e->getComponent<CPoisoned>().timeSinceLastTick >= e->getComponent<CPoisoned>().timeBetweenTicks)
			{
				e->getComponent<CHealth>().current -= e->getComponent<CPoisoned>().poisonDamage;
				if (e == m_player) {
					if (e->hasComponent<CInvincibility>() == true) {
						e->getComponent<CHealth>().current++;
					}
					else {
						m_game->playSound("LinkHurt");
						e->addComponent<CInvincibility>(30);
					}
				}
				if (e->getComponent<CHealth>().current < 0) {
					e->getComponent<CHealth>().current = 0;
				}
				if (e->getComponent<CHealth>().current <= 0)
				{
					e->addComponent<CDying>();
					if (e != m_player) {
						e->removeComponent<CBoundingBox>();
					}
					else {

						m_player->removeComponent<CInput>();
					}
					if (e->hasComponent<CFollowPlayer>())
					{
						e->removeComponent<CFollowPlayer>();
					}
					if (e->hasComponent<CPatrol>())
					{
						e->removeComponent<CPatrol>();
					}
					if (e->hasComponent<CRange>())
					{
						e->removeComponent<CRange>();
					}
					e->removeComponent<CHealth>();
					e->getComponent<CTransform>().velocity = Vec2(0, 0);
					m_game->playSound("EnemyDie");
				}
				e->getComponent<CPoisoned>().poisonDuration--;
				if (e->getComponent<CPoisoned>().poisonDuration <= 0)
				{
					e->removeComponent<CPoisoned>();
				}
				e->getComponent<CPoisoned>().timeSinceLastTick = 0;
			}
			else
			{
				e->getComponent<CPoisoned>().timeSinceLastTick++;
			}
		}
		if (e->hasComponent<CBuffs>())
		{
			if (e->getComponent<CBuffs>().attackBuffed)
			{
				e->getComponent<CBuffs>().attackBuffDuration--;
				if (e->getComponent<CBuffs>().attackBuffDuration <= 0)
				{
					e->getComponent<CBuffs>().attackBuffed = false;
				}
			}
			if (e->getComponent<CBuffs>().speedBuffed)
			{
				e->getComponent<CBuffs>().speedBuffDuration--;
				if (e->getComponent<CBuffs>().speedBuffDuration <= 0)
				{
					e->getComponent<CBuffs>().speedBuffed = false;
				}
			}
		}
		if (e->hasComponent<CDying>())
		{
			if (e->getComponent<CDying>().ticksUntilDeath <= 0)
			{

				e->destroy();
				if (e == m_player) {
					spawnPlayer();
				}
			}
			else
			{
				e->getComponent<CDying>().ticksUntilDeath--;
			}
		}
		if (e->hasComponent<CFalling>())
		{
			e->getComponent<CFalling>().timeFalling++;
			if (e == m_player)
			{
				if (e->getComponent<CFalling>().timeFalling == 35)
				{
					e->destroy();
					spawnPlayer();
				}
			}
		}
	}
	// Implement Invincibility Frames here
}

void Scene_Zelda::sCollision()
{
	bool onTile = false;
	auto tiles = m_entityManager.getEntities("tile");
	auto lands = m_entityManager.getEntities("Land");
	auto water = m_entityManager.getEntities("Water");

	tiles.insert(tiles.end(), lands.begin(), lands.end());
	tiles.insert(tiles.end(), water.begin(), water.end());
	auto npcs = m_entityManager.getEntities("NPC");
	for (auto t : tiles)
	{
		for (std::shared_ptr<Entity> n : npcs)
		{
			// overlap code adapted from assignment 3
			Vec2 overlap = Physics::GetOverlap(n, t);
			if ((overlap.x || overlap.y) && !t->getComponent<CBoundingBox>().blockMove)
			{
				if (t->getComponent<CAnimation>().animation.getName() == "Heart")
				{
					if (n->hasComponent<CHealth>())
					{
						m_game->playSound("GetItem");
						n->getComponent<CHealth>().current = n->getComponent<CHealth>().max;
						t->destroy();
					}
				}
				if (t->getComponent<CAnimation>().animation.getName() == "Spikes")
				{
					if (Physics::IsInside(n->getComponent<CTransform>().pos, t))
					{
						n->getComponent<CHealth>().current--;
						if (n->getComponent<CHealth>().current <= 0)
						{
							n->addComponent<CDying>();
							n->removeComponent<CBoundingBox>();
							if (n->hasComponent<CFollowPlayer>())
							{
								n->removeComponent<CFollowPlayer>();
							}
							if (n->hasComponent<CPatrol>())
							{
								n->removeComponent<CPatrol>();
							}
							if (n->hasComponent<CRange>())
							{
								n->removeComponent<CRange>();
							}
							n->removeComponent<CHealth>();
							n->getComponent<CTransform>().velocity = Vec2(0, 0);
							m_game->playSound("EnemyDie");
						}
					}
				}
			}
			else if ((overlap.x || overlap.y) && t->getComponent<CBoundingBox>().blockMove)
			{
				if (overlap.x < overlap.y)
				{
					// overlap from side. Push them to edge of block

					//overlap from left, push left
					if (n->getComponent<CTransform>().pos.x < t->getComponent<CTransform>().pos.x)
					{
						n->getComponent<CTransform>().pos.x = t->getComponent<CTransform>().pos.x - (t->getComponent<CBoundingBox>().halfSize.x + n->getComponent<CBoundingBox>().halfSize.x);
					}
					// overlap from right, push right
					else {
						n->getComponent<CTransform>().pos.x = t->getComponent<CTransform>().pos.x + (t->getComponent<CBoundingBox>().halfSize.x + n->getComponent<CBoundingBox>().halfSize.x);

					}
				}
				//overlap vertically
				else {
					// collision from above
					if (n->getComponent<CTransform>().pos.y < t->getComponent<CTransform>().pos.y) {
						n->getComponent<CTransform>().pos.y = t->getComponent<CTransform>().pos.y - (t->getComponent<CBoundingBox>().halfSize.y + n->getComponent<CBoundingBox>().halfSize.y);
					}
					// collision from below
					else {
						n->getComponent<CTransform>().pos.y = t->getComponent<CTransform>().pos.y + (t->getComponent<CBoundingBox>().halfSize.y + n->getComponent<CBoundingBox>().halfSize.y);
						n->getComponent<CTransform>().velocity.y = 0;
					}
				}
			}
			Vec2 npcOverlap = Physics::GetOverlap(m_player, n);
			if (npcOverlap.x || npcOverlap.y)
			{
				if (n->hasComponent<CDamage>() && !m_player->hasComponent<CInvincibility>())
				{
					m_player->getComponent<CHealth>().current -= n->getComponent<CDamage>().damage;
					if (m_player->getComponent<CHealth>().current <= 0)
					{
						m_game->playSound("LinkDie");
						m_player->destroy();
						spawnPlayer();
					}
					else {
						m_game->playSound("LinkHurt");
						m_player->addComponent<CInvincibility>(30);
						if (n->getComponent<CBuffs>().attacksPoison)
						{
							auto buffs = n->getComponent<CBuffs>();
							if (m_player->hasComponent<CPoisoned>())
							{
								m_player->getComponent<CPoisoned>().poisonDuration = buffs.poisonDuration;
								m_player->getComponent<CPoisoned>().poisonDamage = buffs.poisonDamage;
								m_player->getComponent<CPoisoned>().timeBetweenTicks = buffs.poisonRate;
							}
							else
							{
								m_player->addComponent<CPoisoned>(buffs.poisonDuration, buffs.poisonDamage, buffs.poisonRate);
							}
							buffs.poisonCharges--;
						}
					}
				}
			}
			bool inRange = Physics::IsInRange(m_player->getComponent<CTransform>().pos, n);
			if (inRange)
			{
				spawnArrow(n, m_player);
			}
			for (std::shared_ptr<Entity> a : m_entityManager.getEntities("pArrow"))
			{
				Vec2 aOverlap = Physics::GetOverlap(n, a);
				if (aOverlap.x || aOverlap.y)
				{
					if (a->hasComponent<CDamage>() && (a->isActive()))
					{
						n->getComponent<CHealth>().current -= a->getComponent<CDamage>().damage;
						a->destroy();
						if (n->getComponent<CHealth>().current <= 0)
						{
							n->destroy();
						}
					}
				}
			}
		}

		bool gInRange = Physics::IsInRange(m_player->getComponent<CTransform>().pos, t);
		if (gInRange)
		{
			auto nPos = t->getComponent<CTransform>().pos;
			if (abs(m_player->getComponent<CTransform>().pos.x - nPos.x) > 5
				|| abs(m_player->getComponent<CTransform>().pos.y - nPos.y) > 5)
			{
				float xDist = m_player->getComponent<CTransform>().pos.x - nPos.x;
				float yDist = m_player->getComponent<CTransform>().pos.y - nPos.y;
				float percentageX = xDist / (abs(xDist) + abs(yDist));
				float percentageY = yDist / (abs(xDist) + abs(yDist));
				m_player->getComponent<CTransform>().pos.x += -percentageX * 2;
				m_player->getComponent<CTransform>().pos.y += -percentageY * 2;
			}
		}
		bool inside = Physics::IsInside(m_player->getComponent<CTransform>().pos, t);
		if (inside)
		{
			auto nVel = t->getComponent<CTransform>().velocity;
			m_player->getComponent<CTransform>().pos.x += nVel.x;
			m_player->getComponent<CTransform>().pos.y += nVel.y;
		}

		for (std::shared_ptr<Entity> a : m_entityManager.getEntities("nArrow"))
		{
			Vec2 aOverlap = Physics::GetOverlap(m_player, a);
			if (aOverlap.x || aOverlap.y)
			{
				if (a->hasComponent<CDamage>() && !m_player->hasComponent<CInvincibility>())
				{
					auto buffs = a->getComponent<CBuffs>();
					if (buffs.attacksPoison) {
						if (m_player->hasComponent<CPoisoned>())
						{
							m_player->getComponent<CPoisoned>().poisonDamage = buffs.poisonDamage;
							m_player->getComponent<CPoisoned>().poisonDuration = buffs.poisonDuration;
							m_player->getComponent<CPoisoned>().timeBetweenTicks = buffs.poisonRate;
						}
						else
						{
							m_player->addComponent<CPoisoned>(buffs.poisonDuration, buffs.poisonDamage, buffs.poisonRate);
						}
					}
					m_player->getComponent<CHealth>().current -= a->getComponent<CDamage>().damage;
					a->destroy();
					if (m_player->getComponent<CHealth>().current <= 0)
					{
						m_game->playSound("LinkDie");
						m_player->destroy();
						spawnPlayer();
					}
					else {
						m_game->playSound("LinkHurt");
						m_player->addComponent<CInvincibility>(30);
					}
				}
				else if (a->isActive())
				{
					m_player->getComponent<CInventory>().arrows += 1;
					a->destroy();
				}
			}
			Vec2 arrowOverlap = Physics::GetOverlap(t, a);
			if (t->getComponent<CBoundingBox>().blockVision && (arrowOverlap.x || arrowOverlap.y))
			{
				a->getComponent<CTransform>().velocity = Vec2(0, 0);
				a->removeComponent<CDamage>();
				if (a->hasComponent<CSpecialArrow>()) {
					if (a->getComponent<CSpecialArrow>().spawnGravity == true) {
						t->addComponent<CAnimation>(m_game->assets().getAnimation("GravityWell"), true);
						t->addComponent<CBoundingBox>(m_game->assets().getAnimation("GravityWell").getSize(), false, false);

						t->addComponent<CRange>(m_game->assets().getAnimation("GravityWell").getSize(), 10);
						t->addComponent<CLifeSpan>(300, m_currentFrame);
					}

					if (a->getComponent<CSpecialArrow>().moveTile == true) {
						if (t->tag() != "Land" && t->tag() != "Water" && t->tag() != "decoration")
						{
							t->getComponent<CTransform>().velocity.x = (1 * rand() % 2) * (-1 * rand() % 2);
							t->getComponent<CTransform>().velocity.y = (1 * rand() % 2) * (-1 * rand() % 2);
						}
					}
				}
			}

		}

		for (std::shared_ptr<Entity> a : m_entityManager.getEntities("pArrow"))
		{
			Vec2 aOverlap = Physics::GetOverlap(m_player, a);
			if (aOverlap.x || aOverlap.y)
			{
				if (!a->hasComponent<CDamage>() && (a->isActive()))
				{
					//add arrow to inventory
					m_player->getComponent<CInventory>().arrows += 1;
					a->destroy();
				}
			}
			Vec2 arrowOverlap = Physics::GetOverlap(t, a);
			if (t->getComponent<CBoundingBox>().blockVision && (arrowOverlap.x || arrowOverlap.y))
			{
				a->getComponent<CTransform>().velocity = Vec2(0, 0);
				a->removeComponent<CDamage>();
			}
		}
		if (t->getComponent<CBoundingBox>().blockMove || t->getComponent<CAnimation>().animation.getName() == "Heart")
		{
			Vec2 pOverlap = Physics::GetOverlap(m_player, t);
			// player overlaps with tile; push them back out
			if (pOverlap.x || pOverlap.y)
			{
				if (t->getComponent<CAnimation>().animation.getName() == "Heart")
				{
					m_game->playSound("GetItem");
					m_player->getComponent<CHealth>().current = m_player->getComponent<CHealth>().max;
					t->destroy();
				}
				if (t->getComponent<CAnimation>().animation.getName() == "Black")
				{

				}
				else
				{
					if (pOverlap.x < pOverlap.y)
					{
						// overlap from side. Push them to edge of block

						//overlap from left, push left
						if (m_player->getComponent<CTransform>().pos.x < t->getComponent<CTransform>().pos.x)
						{
							m_player->getComponent<CTransform>().pos.x = t->getComponent<CTransform>().pos.x - (t->getComponent<CBoundingBox>().halfSize.x + m_player->getComponent<CBoundingBox>().halfSize.x);
						}
						// overlap from right, push right
						else {
							m_player->getComponent<CTransform>().pos.x = t->getComponent<CTransform>().pos.x + (t->getComponent<CBoundingBox>().halfSize.x + m_player->getComponent<CBoundingBox>().halfSize.x);

						}
					}
					//overlap vertically
					else {
						// collision from above
						if (m_player->getComponent<CTransform>().pos.y < t->getComponent<CTransform>().pos.y) {
							m_player->getComponent<CTransform>().pos.y = t->getComponent<CTransform>().pos.y - (t->getComponent<CBoundingBox>().halfSize.y + m_player->getComponent<CBoundingBox>().halfSize.y);
						}
						// collision from below
						else {
							m_player->getComponent<CTransform>().pos.y = t->getComponent<CTransform>().pos.y + (t->getComponent<CBoundingBox>().halfSize.y + m_player->getComponent<CBoundingBox>().halfSize.y);
							m_player->getComponent<CTransform>().velocity.y = 0;
						}
					}
				}
			}

		}
		else if (t->getComponent<CAnimation>().animation.getName() == "Spikes")
		{
			if (Physics::IsInside(m_player->getComponent<CTransform>().pos, t) && !m_player->hasComponent<CInvincibility>())
			{
				m_player->getComponent<CHealth>().current -= 1;
				if (m_player->getComponent<CHealth>().current <= 0)
				{
					m_game->playSound("LinkDie");
					m_player->destroy();
					spawnPlayer();
				}
				else {
					m_game->playSound("LinkHurt");
					m_player->addComponent<CInvincibility>(30);
				}
			}
		}
	}
	for (auto l : m_entityManager.getEntities("tile"))
	{
		// check if overlap with any tiles
		// if there is no overlap, player has fallen off the map
		auto overlap = Physics::GetOverlap(m_player, l);
		if (overlap.x != 0 || overlap.y != 0)
		{
			onTile = true;
		}
	}
	for (auto l : m_entityManager.getEntities("Land"))
	{
		// check if overlap with any land tiles
		// if there is no overlap, player has fallen off the map
		auto overlap = Physics::GetOverlap(m_player, l);
		if (overlap.x != 0 || overlap.y != 0)
		{
			onTile = true;
		}
	}
	if (!onTile)
	{
		m_player->getComponent<CTransform>().velocity = Vec2(0, 0);
		if (!m_player->hasComponent<CFalling>())
		{
			m_player->removeComponent<CInput>();
			m_player->addComponent<CFalling>();
			m_player->removeComponent<CAnimation>();
			m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Falling"), true);
			m_player->getComponent<CState>().state = "Falling";
		}
	}
	// if a sword exists, check collisions
	auto swords = m_entityManager.getEntities("Sword");
	if (swords.size() != 0)
	{
		// there should only ever be one sword max at a time, so it should be fine to just take the first (and only) entry in the array
		auto sword = swords[0];
		for (std::shared_ptr<Entity> n : npcs)
		{
			Vec2 overlap = Physics::GetOverlap(n, sword);
			if (overlap.x || overlap.y)
			{
				if (n->hasComponent<CHealth>() && currentFrame() - sword->getComponent<CLifeSpan>().frameCreated == 5)
				{
					n->getComponent<CHealth>().current -= sword->getComponent<CDamage>().damage;
					if (n->getComponent<CHealth>().current < 0) {
						n->getComponent<CHealth>().current = 0;
					}
					m_game->playSound("EnemyHit");
					if (sword->getComponent<CBuffs>().attacksPoison)
					{
						auto poison = n->getComponent<CPoisoned>();
						auto buffs = sword->getComponent<CBuffs>();
						if (n->hasComponent<CPoisoned>())
						{
							poison.poisonDamage = buffs.poisonDamage;
							poison.poisonDuration = buffs.poisonDuration;
							poison.timeBetweenTicks = buffs.poisonRate;
						}
						else {
							n->addComponent<CPoisoned>(buffs.poisonDuration, buffs.poisonDamage, buffs.poisonRate);
						}
						m_player->getComponent<CBuffs>().poisonCharges--;
						if (m_player->getComponent<CBuffs>().poisonCharges <= 0)
						{
							m_player->getComponent<CBuffs>().attacksPoison = false;
						}
					}
					if (n->getComponent<CHealth>().current <= 0)
					{
						if (!n->hasComponent<CBoss>()) {
							n->addComponent<CDying>();
							n->removeComponent<CBoundingBox>();
							if (n->hasComponent<CFollowPlayer>())
							{
								n->removeComponent<CFollowPlayer>();
							}
							if (n->hasComponent<CPatrol>())
							{
								n->removeComponent<CPatrol>();
							}
							if (n->hasComponent<CRange>())
							{
								n->removeComponent<CRange>();
							}
							n->removeComponent<CHealth>();
							n->getComponent<CTransform>().velocity = Vec2(0, 0);
							m_game->playSound("EnemyDie");
						}
						else if (m_Boss->getComponent<CBoss>().healthBars < 1) {
							n->addComponent<CDying>();
							n->removeComponent<CBoundingBox>();
							if (n->hasComponent<CFollowPlayer>())
							{
								n->removeComponent<CFollowPlayer>();
							}
							if (n->hasComponent<CPatrol>())
							{
								n->removeComponent<CPatrol>();
							}
							if (n->hasComponent<CRange>())
							{
								n->removeComponent<CRange>();
							}
							n->removeComponent<CHealth>();
							n->getComponent<CTransform>().velocity = Vec2(0, 0);
							m_game->playSound("EnemyDie");
						}
					}
				}
			}
		}
	}
	if (m_playerConfig.hasTorch)
	{
		filterImage = baseFilter;
		auto hSize = m_player->getComponent<CLight>().light;
		auto pos = m_player->getComponent<CTransform>().pos;
		for (auto x = (pos.x - hSize); x <= (pos.x + hSize); x++)
		{
			for (auto y = (pos.y - hSize); y <= (pos.y + hSize); y++)
			{
				auto upperx = m_game->window().getView().getCenter().x - m_game->window().getSize().x / 2;
				auto uppery = m_game->window().getView().getCenter().y - m_game->window().getSize().y / 2;
				auto filter = Vec2(x - upperx, y - uppery);
				if (filter.x > 0 && filter.x < filterImage.getSize().x && filter.y > 0 && filter.y < filterImage.getSize().y)
				{
					filterImage.setPixel(filter.x, filter.y, sf::Color::Transparent);
				}
			}
		}
		auto it = lightPixels.begin();
		auto upperx = m_game->window().getView().getCenter().x - m_game->window().getSize().x / 2;
		auto uppery = m_game->window().getView().getCenter().y - m_game->window().getSize().y / 2;
		while (it != lightPixels.end())
		{
			auto posIt = *it;
			auto filter = Vec2(posIt.x - upperx, posIt.y - uppery);
			if (filter.x > 0 && filter.x < filterImage.getSize().x && filter.y > 0 && filter.y < filterImage.getSize().y)
			{
				filterImage.setPixel(filter.x, filter.y, sf::Color::Transparent);
			}
			it++;
		}
	}
	//Here we do a quick check to make sure there is a boss in a level and then
	//Check to see if the boss has been flagged dead during this update
	//If flagged dead we check if the player has already completed a higher level
	//If they have then this won't update anything otherwise this level is wrote
	//as a int to a file to be read by scene_map
	if (bossExists) {
		if (m_Boss->getComponent<CHealth>().current < 1 && m_Boss->getComponent<CBoss>().healthBars > 0) {
			m_Boss->getComponent<CBoss>().healthBars -= 1;
			m_Boss->removeComponent<CDying>();
			m_Boss->unDestroy();
			m_Boss->getComponent<CHealth>().current = m_Boss->getComponent<CHealth>().max;
		}
	}
	if (bossExists && m_Boss->isActive() == false) {
		if (m_levelPath.find("customLevels/") != std::string::npos) {
			m_game->assets().getSound("MusicLevel").stop();
			m_game->changeScene("MAP", std::make_shared<Scene_Map>(m_game, "customLevels/overworld.txt"));

		}
		else {
			endLevelVictory(findCurrentLevel());
		}
	}
	if (m_entityManager.getEntities("NPC").size() == 0) {
		if (m_levelPath.find("customLevels/") != std::string::npos) {
			m_game->assets().getSound("MusicLevel").stop();
			m_game->changeScene("MAP", std::make_shared<Scene_Map>(m_game, "customLevels/overworld.txt"));

		}
		else {
			endLevelVictory(findCurrentLevel());
		}

	}

}
void Scene_Zelda::sAnimation()
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
	auto swordCheck = m_entityManager.getEntities("Sword");
	if (swordCheck.size() == 1) {
		auto Sword = swordCheck[0];
		const auto& playerTransform = m_player->getComponent<CTransform>();


		if (m_player->getComponent<CTransform>().facing.x > 0) {
			Sword->addComponent<CAnimation>(m_game->assets().getAnimation("SwordRight"), true);
			Sword->addComponent<CBoundingBox>(m_game->assets().getAnimation("SwordRight").getSize());
			// 47 is used here to offset the sword animation
			// typically we'd use 48, but we want 1 less to make it so that if an enemy is on top of the player that the player can still hit the enemy
			// much improves game feel
			Sword->addComponent<CTransform>(Vec2(playerTransform.pos.x + 47, playerTransform.pos.y));
			m_player->addComponent<CAnimation>(m_game->assets().getAnimation("AtkRight"), true);

		}
		else if (m_player->getComponent<CTransform>().facing.x < 0) {
			Sword->addComponent<CAnimation>(m_game->assets().getAnimation("SwordRight"), true);
			Sword->addComponent<CBoundingBox>(m_game->assets().getAnimation("SwordRight").getSize());
			Sword->addComponent<CTransform>(Vec2(playerTransform.pos.x - 47, playerTransform.pos.y));
			Sword->getComponent<CTransform>().scale.x = -1;
			m_player->addComponent<CAnimation>(m_game->assets().getAnimation("AtkRight"), true);

		}

		if (m_player->getComponent<CTransform>().facing.y > 0) {
			Sword->addComponent<CAnimation>(m_game->assets().getAnimation("SwordUp"), true);
			Sword->addComponent<CBoundingBox>(m_game->assets().getAnimation("SwordUp").getSize());
			Sword->addComponent<CTransform>(Vec2(playerTransform.pos.x, playerTransform.pos.y - 47));
			m_player->addComponent<CAnimation>(m_game->assets().getAnimation("AtkUp"), true);

		}
		else if (m_player->getComponent<CTransform>().facing.y < 0) {
			Sword->addComponent<CAnimation>(m_game->assets().getAnimation("SwordDown"), true);
			Sword->addComponent<CBoundingBox>(m_game->assets().getAnimation("SwordDown").getSize());
			Sword->addComponent<CTransform>(Vec2(playerTransform.pos.x, playerTransform.pos.y + 47));
			m_player->addComponent<CAnimation>(m_game->assets().getAnimation("AtkDown"), true);

		}



	}

	for (auto e : m_entityManager.getEntities())
	{
		if (e->hasComponent<CAnimation>() && !e->hasComponent<CMimic>())
		{
			e->getComponent<CAnimation>().animation.update();

		}
		if (e->getComponent<CAnimation>().animation.hasEnded() && !e->getComponent<CAnimation>().repeat) {
			e->destroy();
		}
	}
}

void Scene_Zelda::OrientateCharacter(std::shared_ptr<Entity> entity, int xDirection = 1, int yDirection = 0) {
	entity->getComponent<CTransform>().scale.x = xDirection;
	entity->getComponent<CTransform>().facing.x = xDirection;
	entity->getComponent<CTransform>().facing.y = yDirection;
}

void Scene_Zelda::sCamera()
{

	// get the current view, which we will modify in the if-statement below
	sf::View view = m_game->window().getView();
	m_prevView = view.getCenter();

	if (m_follow)
	{
		// calculate view for player follow camera
		view.setCenter(m_player->getComponent<CTransform>().pos.x, m_player->getComponent<CTransform>().pos.y);
	}
	else

	{
		// calculate view for room-based camera
		auto windowSize = m_game->window().getSize();
		int xDimension = int(m_player->getComponent<CTransform>().pos.x / int(windowSize.x)) * int(windowSize.x) + int(windowSize.x) / 2;
		int yDimension = int(m_player->getComponent<CTransform>().pos.y / int(windowSize.y)) * int(windowSize.y) + int(windowSize.y / 2);

		view.setCenter(xDimension, yDimension);

		if (m_player->getComponent<CTransform>().pos.y < 0 && m_player->getComponent<CTransform>().pos.x < 0) {
			view.setCenter(xDimension - 1280, yDimension - 768);
		}
		else if (m_player->getComponent<CTransform>().pos.x < 0) {
			view.setCenter(xDimension - 1280, yDimension);
		}
		else if (m_player->getComponent<CTransform>().pos.y < 0) {
			view.setCenter(xDimension, yDimension - 768);

		}
	}

	// then set the window view
	m_game->window().setView(view);

	//Parallax Background based on camera position perspectice
	sf::Vector2f deltaVector = view.getCenter() - m_prevView;
	//Move Clouds due to 'wind'
	m_parallaxXStep += 3;
	m_parallaxYStep += 2;
	if (std::trunc(deltaVector.x) != 0)
	{
		m_parallaxXStep += std::trunc(deltaVector.x);
	}
	if (std::trunc(deltaVector.y) != 0)
	{
		m_parallaxYStep += std::trunc(deltaVector.y);

	}
	if (std::abs(m_parallaxXStep) >= 3000)
	{
		m_parallaxXStep = 0;
	}
	if (std::abs(m_parallaxYStep) >= 1630)
	{
		m_parallaxYStep = 0;
	}
	m_background1.setPosition(view.getCenter() - sf::Vector2f((m_game->window().getSize().x / 2 + 400) + m_parallaxXStep * 0.1, (m_game->window().getSize().y / 2 + 200) + m_parallaxYStep * 0.1));
	m_background2.setPosition(view.getCenter() - sf::Vector2f(m_game->window().getSize().x / 2 + 20, m_game->window().getSize().y / 2 + 32));

}

void Scene_Zelda::onEnd()
{
	m_game->assets().getSound("MusicLevel").stop();
	m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game));
	m_game->loopSound("MusicTitle");

}

void Scene_Zelda::sRender()
{
	if (!m_paused) {
		m_game->window().clear(sf::Color(135, 206, 235));
		//Draw Parallax Background
		m_game->window().draw(m_background2);
		m_game->window().draw(m_background1);
		sf::RectangleShape tick({ 1.0f, 6.0f });
		tick.setFillColor(sf::Color::Black);

		// draw all Entity textures / animations
		if (m_drawTextures)
		{
			for (auto e : m_entityManager.getEntities("Land"))
			{
				auto& transform = e->getComponent<CTransform>();
				sf::Color c = sf::Color::White;
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
			for (auto e : m_entityManager.getEntities("Water"))
			{
				auto& transform = e->getComponent<CTransform>();
				sf::Color c = sf::Color::White;
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
			for (auto e : m_entityManager.getEntities("decoration"))
			{
				auto& transform = e->getComponent<CTransform>();
				sf::Color c = sf::Color::White;
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
				if (e->tag() != "Land" && e->tag() != "decoration" && e->tag() != "Water")
				{
					auto& transform = e->getComponent<CTransform>();
					sf::Color c = sf::Color::White;
					if (e->hasComponent<CInvincibility>())
					{
						c = sf::Color(255, 255, 255, 128);
					}
					if (e->hasComponent<CDying>())
					{
						sf::Shader deathShader;
						deathShader.loadFromFile("Death_Shader.frag", sf::Shader::Fragment);
						deathShader.setUniform("framesLeft", e->getComponent<CDying>().ticksUntilDeath);
						auto& animation = e->getComponent<CAnimation>().animation;
						animation.getSprite().setRotation(transform.angle);
						animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
						animation.getSprite().setScale(transform.scale.x, transform.scale.y);
						animation.getSprite().setColor(c);
						m_game->window().draw(animation.getSprite(), &deathShader);
					}
					else if (e->hasComponent<CPoisoned>())
					{
						sf::Shader poisonShader;
						poisonShader.loadFromFile("Poison_Shader.frag", sf::Shader::Fragment);
						poisonShader.setUniform("time", time.getElapsedTime().asSeconds());
						auto& animation = e->getComponent<CAnimation>().animation;
						animation.getSprite().setRotation(transform.angle);
						animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
						animation.getSprite().setScale(transform.scale.x, transform.scale.y);
						animation.getSprite().setColor(c);
						m_game->window().draw(animation.getSprite(), &poisonShader);
					}
					else if (e->hasComponent<CAnimation>())
					{
						auto& animation = e->getComponent<CAnimation>().animation;
						animation.getSprite().setRotation(transform.angle);
						animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
						animation.getSprite().setScale(transform.scale.x, transform.scale.y);
						animation.getSprite().setColor(c);
						m_game->window().draw(animation.getSprite());
					}
				}
			}

			for (auto e : m_entityManager.getEntities())
			{
				auto& transform = e->getComponent<CTransform>();
				if (e->hasComponent<CHealth>() && !e->hasComponent<CInput>() && !e->hasComponent<CFalling>())
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

		auto upperx = m_game->window().getView().getCenter().x - m_game->window().getSize().x / 2;
		auto uppery = m_game->window().getView().getCenter().y - m_game->window().getSize().y / 2;
		sf::Texture texture;
		texture.loadFromImage(filterImage);
		sf::Sprite sprite;
		sprite.setTexture(texture, true);
		sprite.setPosition(upperx, uppery);
		m_game->window().draw(sprite);

		// display HUD
		sf::View defaultView = m_game->window().getView();
		sf::View HUD = m_game->window().getDefaultView();
		m_game->window().setView(HUD);

		sf::Text arrows;
		arrows.setString(std::to_string(m_player->getComponent<CInventory>().arrows));
		arrows.setPosition(sf::Vector2f(66, 650));
		arrows.setFont(m_game->assets().getFont("Computer"));
		arrows.setFillColor(sf::Color::Red);
		arrows.setOutlineThickness(2);
		arrows.setOutlineColor(sf::Color::Black);
		m_game->window().draw(arrows);

		sf::RectangleShape arrowBox({ 32, 32 });
		arrowBox.setPosition(sf::Vector2f(68, 686));
		arrowBox.setOutlineColor(sf::Color::Red);
		arrowBox.setOutlineThickness(2);
		arrowBox.setFillColor(sf::Color(255, 255, 0, 75));
		m_game->window().draw(arrowBox);

		auto arrowAnimation = m_game->assets().getAnimation("Arrow");
		arrowAnimation.getSprite().setPosition(sf::Vector2f(84, 702));
		m_game->window().draw(arrowAnimation.getSprite());


		sf::Text hPotions;
		hPotions.setString(std::to_string(m_player->getComponent<CInventory>().healthPotions));
		hPotions.setPosition(sf::Vector2f(124, 650));
		hPotions.setFont(m_game->assets().getFont("Computer"));
		hPotions.setFillColor(sf::Color::Red);
		hPotions.setOutlineThickness(2);
		hPotions.setOutlineColor(sf::Color::Black);
		m_game->window().draw(hPotions);

		sf::RectangleShape healthPotionBox({ 32, 32 });
		healthPotionBox.setPosition(sf::Vector2f(116, 686));
		healthPotionBox.setOutlineColor(sf::Color::Red);
		healthPotionBox.setOutlineThickness(2);
		healthPotionBox.setFillColor(sf::Color(255, 255, 0, 75));
		m_game->window().draw(healthPotionBox);

		auto hPotionAnimation = m_game->assets().getAnimation("HealthPotion");
		hPotionAnimation.getSprite().setPosition(sf::Vector2f(132, 702));
		m_game->window().draw(hPotionAnimation.getSprite());


		sf::Text sPotions;
		sPotions.setString(std::to_string(m_player->getComponent<CInventory>().strengthPotions));
		sPotions.setPosition(sf::Vector2f(172, 650));
		sPotions.setFont(m_game->assets().getFont("Computer"));
		sPotions.setFillColor(sf::Color::Red);
		sPotions.setOutlineThickness(2);
		sPotions.setOutlineColor(sf::Color::Black);
		m_game->window().draw(sPotions);

		sf::RectangleShape strengthPotionBox({ 32, 32 });
		strengthPotionBox.setPosition(sf::Vector2f(164, 686));
		strengthPotionBox.setOutlineColor(sf::Color::Red);
		strengthPotionBox.setOutlineThickness(2);
		strengthPotionBox.setFillColor(sf::Color(255, 255, 0, 75));
		m_game->window().draw(strengthPotionBox);

		auto sPotionAnimation = m_game->assets().getAnimation("StrengthPotion");
		sPotionAnimation.getSprite().setPosition(sf::Vector2f(180, 702));
		m_game->window().draw(sPotionAnimation.getSprite());


		sf::Text spPotions;
		spPotions.setString(std::to_string(m_player->getComponent<CInventory>().speedPotions));
		spPotions.setPosition(sf::Vector2f(220, 650));
		spPotions.setFont(m_game->assets().getFont("Computer"));
		spPotions.setFillColor(sf::Color::Red);
		spPotions.setOutlineThickness(2);
		spPotions.setOutlineColor(sf::Color::Black);
		m_game->window().draw(spPotions);

		sf::RectangleShape speedPotionBox({ 32, 32 });
		speedPotionBox.setPosition(sf::Vector2f(212, 686));
		speedPotionBox.setOutlineColor(sf::Color::Red);
		speedPotionBox.setOutlineThickness(2);
		speedPotionBox.setFillColor(sf::Color(255, 255, 0, 75));
		m_game->window().draw(speedPotionBox);

		auto spPotionAnimation = m_game->assets().getAnimation("SpeedPotion");
		spPotionAnimation.getSprite().setPosition(sf::Vector2f(228, 702));
		m_game->window().draw(spPotionAnimation.getSprite());


		sf::Text poisonFlasks;
		poisonFlasks.setString(std::to_string(m_player->getComponent<CInventory>().poisonFlasks));
		poisonFlasks.setPosition(sf::Vector2f(268, 650));
		poisonFlasks.setFont(m_game->assets().getFont("Computer"));
		poisonFlasks.setFillColor(sf::Color::Red);
		poisonFlasks.setOutlineThickness(2);
		poisonFlasks.setOutlineColor(sf::Color::Black);
		m_game->window().draw(poisonFlasks);

		sf::RectangleShape poisonFlaskBox({ 32, 32 });
		poisonFlaskBox.setPosition(sf::Vector2f(260, 686));
		poisonFlaskBox.setOutlineColor(sf::Color::Red);
		poisonFlaskBox.setOutlineThickness(2);
		poisonFlaskBox.setFillColor(sf::Color(255, 255, 0, 75));
		m_game->window().draw(poisonFlaskBox);

		auto poisonAnimation = m_game->assets().getAnimation("PoisonFlask");
		poisonAnimation.getSprite().setPosition(sf::Vector2f(276, 702));
		m_game->window().draw(poisonAnimation.getSprite());

		sf::Text Lifes;
		Lifes.setString("X " + std::to_string(playerLives));
		Lifes.setPosition(sf::Vector2f(75, 38));
		Lifes.setFont(m_game->assets().getFont("Computer"));
		Lifes.setFillColor(sf::Color::Red);
		Lifes.setOutlineThickness(2);
		Lifes.setOutlineColor(sf::Color::Black);
		m_game->window().draw(Lifes);

		auto lifeAnimation = m_game->assets().getAnimation("StandDown");
		lifeAnimation.getSprite().setPosition(sf::Vector2f(45, 45));
		m_game->window().draw(lifeAnimation.getSprite());


		auto& h = m_player->getComponent<CHealth>();
		Vec2 size(256, 16);
		sf::RectangleShape rect({ size.x, size.y });
		rect.setPosition(56, 620);
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

		for (int i = 1; i < h.max; i++)
		{
			tick.setPosition(rect.getPosition() + sf::Vector2f(i * 256 * 1 / h.max, 0));
			m_game->window().draw(tick);
		}
		m_game->window().setView(defaultView);


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

				if (e->hasComponent<CRange>())
				{
					auto& box = e->getComponent<CRange>();
					auto& transform = e->getComponent<CTransform>();
					sf::RectangleShape rect;
					rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
					rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
					rect.setPosition(transform.pos.x, transform.pos.y);
					rect.setFillColor(sf::Color(0, 0, 0, 0));
					rect.setOutlineColor(sf::Color::Magenta);
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
	else {
		pausedRender();
	}
}

/*
This is the render function that's called from the normal sRender.
When paused this will create a black screen with text drawn on top
dependent on which menu page bool is currently set to true. It uses
the code from Scene_Menu to draw the black screen.
*/
void Scene_Zelda::pausedRender()
{


	if (m_paused && hotkeyMenu) {

		m_menuStrings.clear();
		m_menuStrings.push_back("QUIT");
		m_menuStrings.push_back("PAUSE");
		m_menuStrings.push_back("TOGGLE_FOLLOW");
		m_menuStrings.push_back("TOGGLE_TEXTURE");
		m_menuStrings.push_back("TOGGLE_COLLISION");
		m_menuStrings.push_back("MOVE_UP");
		m_menuStrings.push_back("MOVE_LEFT");
		m_menuStrings.push_back("MOVE_DOWN");
		m_menuStrings.push_back("MOVE_RIGHT");
		m_menuStrings.push_back("ATTACK");
		m_menuText.setString("Change Hotkey");
	}
	else if (assigningHotkey && m_paused) {


		m_menuStrings.clear();


		m_menuStrings.push_back("Assign Hotkey " + SelectedMenuOption);
		m_menuText.setString("Paused Menu");

	}
	else if (musicVolume && m_paused) {


		m_menuStrings.clear();


		m_menuStrings.push_back("Current Music Volume is " + std::to_string(musicLevel));
		m_menuText.setString("Set Music Volume Level");

	}
	else if (soundEffectVolume && m_paused) {


		m_menuStrings.clear();


		m_menuStrings.push_back("Current Sound Effect \n Volume is " + std::to_string(soundEffectLevel));
		m_menuText.setString("Set Sound Effect Volume Level");

	}
	else if (inventory && m_paused) {


		m_menuStrings.clear();


		m_menuStrings.push_back("Arrows x" + std::to_string(m_player->getComponent<CInventory>().arrows));
		m_menuStrings.push_back("Health Potions x" + std::to_string(m_player->getComponent<CInventory>().healthPotions));
		m_menuStrings.push_back("Strength Potions x" + std::to_string(m_player->getComponent<CInventory>().strengthPotions));
		m_menuStrings.push_back("Speed Potions x" + std::to_string(m_player->getComponent<CInventory>().speedPotions));
		m_menuStrings.push_back("Poison Flasks x" + std::to_string(m_player->getComponent<CInventory>().poisonFlasks));

		m_menuText.setString("Inventory");

	}
	else if (gameDifficulty && m_paused) {


		m_menuStrings.clear();

		if (difficulty == 1) {
			m_menuText.setString("Set Difficulty Level \nCurrent- Normal");

		}
		else if (difficulty == 2) {
			m_menuText.setString("Set Difficulty Level \nCurrent- Easy");

		}
		else if (difficulty == 0.5) {
			m_menuText.setString("Set Difficulty Level \nCurrent- Hard");

		}
		m_menuStrings.push_back("\nEasy");
		m_menuStrings.push_back("\nNormal");
		m_menuStrings.push_back("\nHard");


	}
	else if (m_paused) {


		m_menuStrings.clear();

		m_menuStrings.push_back("Inventory");
		m_menuStrings.push_back("Change Hotkey");
		m_menuStrings.push_back("Music Volume");
		m_menuStrings.push_back("Sound Effects Volume");
		m_menuStrings.push_back("Game Difficulty");
		m_menuText.setString("Paused Menu");

	}
	sf::View view = m_game->window().getView();

	view.setCenter(view.getSize().x / 2, view.getSize().y / 2);
	m_game->window().setView(view);
	m_game->window().clear(sf::Color(0, 0, 0));

	// draw the game title in the top-left of the screen
	m_menuText.setCharacterSize(48);

	m_menuText.setFillColor(sf::Color(100, 100, 100));
	m_menuText.setPosition(sf::Vector2f(10, 10));
	m_menuText.setFont(m_game->assets().getFont("PiecesOfEight"));
	m_game->window().draw(m_menuText);
	if (m_paused && hotkeyMenu) {
		m_menuText.setCharacterSize(36);
	}
	// draw all of the menu options
	for (size_t i = 0; i < m_menuStrings.size(); i++)
	{
		m_menuText.setString(m_menuStrings[i]);
		m_menuText.setFillColor(i == m_selectedMenuOption ? sf::Color::White : sf::Color(100, 100, 100));
		if (m_paused && hotkeyMenu) {
			m_menuText.setPosition(sf::Vector2f(10, 110 + i * 50));
		}
		else {
			m_menuText.setPosition(sf::Vector2f(10, 110 + i * 72));
		}
		m_game->window().draw(m_menuText);
	}
	// draw the controls in the bottom-left
	m_menuText.setCharacterSize(20);
	m_menuText.setFillColor(sf::Color(100, 100, 100));
	m_menuText.setString("UP: W     DOWN: S      BACK: A     SELECT: D (if applicable)");
	m_menuText.setPosition(sf::Vector2f(10, 690));
	m_game->window().draw(m_menuText);
}


/*
Change difficulty takes in a string that specifies the difficulty to
adjust the game too. It sets the difficulty float based on difficulty
and this is used here to set your hp and in spawn sword to scale the
sword damage by the same amount.
*/
void Scene_Zelda::changeDifficulty(std::string newDifficulty) {

	if (newDifficulty == "\nEasy") {
		difficulty = 2;
		m_player->addComponent<CHealth>(m_playerConfig.HEALTH * difficulty, (float(m_player->getComponent<CHealth>().current) / float(m_player->getComponent<CHealth>().max)) * m_playerConfig.HEALTH * difficulty);

	}
	else if (newDifficulty == "\nNormal") {
		difficulty = 1;
		m_player->addComponent<CHealth>(m_playerConfig.HEALTH * difficulty, (float(m_player->getComponent<CHealth>().current) / float(m_player->getComponent<CHealth>().max)) * m_playerConfig.HEALTH * difficulty);

	}
	else if (newDifficulty == "\nHard") {

		difficulty = 0.5;
		m_player->addComponent<CHealth>(m_playerConfig.HEALTH * difficulty, (float(m_player->getComponent<CHealth>().current) / float(m_player->getComponent<CHealth>().max)) * m_playerConfig.HEALTH * difficulty);


	}
	if (m_player->getComponent<CHealth>().current > m_player->getComponent<CHealth>().max) {
		m_player->getComponent<CHealth>().current = m_player->getComponent<CHealth>().max;
	}
	if (m_player->getComponent<CHealth>().current == 0) {
		m_player->getComponent<CHealth>().current = 1;
	}

}
/*
This takes in the new int value for the sound to be set to for either
music or sound effects as well as the string telling it which of the
two it's supposed to be looking for.

It gets the sound map from the assets and then iterates through it checking
for the specific sound types which it then sets the volume for

*/
void Scene_Zelda::setSoundLevel(int newVolume, std::string SoundType) {
	for (auto sound : m_game->assets().returnSoundMap()) {
		if (SoundType == "Music" && !sound.first.find("Music")) {

			m_game->assets().getSound(sound.first).setVolume(newVolume);
		}
		else if (SoundType == "SoundEffect" && sound.first.find("Music")) {

			m_game->assets().getSound(sound.first).setVolume(newVolume);
		}
	}
}
/*
This is a shortcut for sDoAction for navigating through the menu
pages by setting the current bool to true for the page you're on

This also sets the current highlighted option to 0 so it always
bumps you up to the start of the list.
*/
void Scene_Zelda::navigateMenu(std::string newPage) {

	hotkeyMenu = false;
	assigningHotkey = false;
	musicVolume = false;
	soundEffectVolume = false;
	gameDifficulty = false;
	inventory = false;
	m_selectedMenuOption = 0;
	if (newPage == "hotkeyMenu") {
		hotkeyMenu = true;
	}
	else if (newPage == "assigningHotkey") {
		assigningHotkey = true;
	}
	else if (newPage == "musicVolume") {
		musicVolume = true;
	}
	else if (newPage == "soundEffectVolume") {
		soundEffectVolume = true;
	}
	else if (newPage == "gameDifficulty") {
		gameDifficulty = true;
	}
	else if (newPage == "inventory") {
		inventory = true;
	}
}

void Scene_Zelda::MakeBoss(std::shared_ptr<Entity> newBoss, std::string animation, bool poison, bool gravity, bool moveTile) {


	newBoss->addComponent<CRange>(m_game->assets().getAnimation(animation).getSize(), 5);

	if (poison) {
		addBossPoison(newBoss);
	}
	if (gravity) {
		newBoss->getComponent<CBoss>().gravityArrows = true;
	}
	if (moveTile) {
		newBoss->getComponent<CBoss>().moveTileArrows = true;
	}


	newBoss->getComponent<CBoss>().levelBoss = findCurrentLevel();
	m_Boss = newBoss;
	bossExists = true;
}

int Scene_Zelda::findCurrentLevel() {
	for (int i = 0; i < 100; i++) {
		if ((m_levelPath.find("level" + std::to_string(i)) != std::string::npos)) {
			currentLevel = i;
		}
	}
	return currentLevel;
}
void Scene_Zelda::endLevelVictory(int levelCompleted) {
	std::ifstream inFile("LevelsCompleted.txt");
	std::string Line;
	std::getline(inFile, Line);
	if (std::stoi(Line) < levelCompleted) {
		std::ofstream controlsFile("LevelsCompleted.txt", std::ios::trunc);

		controlsFile << levelCompleted;

	}

	m_game->assets().getSound("MusicLevel").stop();
	if (levelCompleted == 3)
	{
		m_game->changeScene("Victory", std::make_shared<Scene_Victory>(m_game));
	}
	else
	{
		m_game->changeScene("Map", std::make_shared<Scene_Map>(m_game, "campaignLevels/overworld.txt"));
	}
}
void Scene_Zelda::addBossPoison(std::shared_ptr<Entity> newBoss) {

	newBoss->getComponent<CBoss>().poisonArrows = true;
	newBoss->getComponent<CBuffs>().poisonCharges = 20;
	newBoss->getComponent<CBuffs>().poisonDamage = 1;
	newBoss->getComponent<CBuffs>().poisonRate = 3;
	newBoss->getComponent<CBuffs>().poisonDuration = 5;
}
