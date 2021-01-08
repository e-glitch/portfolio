#include "Common.h"
#include "Scene_Editor.h"
#include "Scene_Zelda.h"
#include "Scene_Editor_Menu.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include <vector>
#include <cmath>
#include <algorithm>
#include "Physics.h"

Scene_Editor::Scene_Editor(GameEngine* game, const std::string& levelPath)
	: Scene(game)
	, m_levelPath(levelPath)
{
	init(m_levelPath);
	
}

void Scene_Editor::init(const std::string& levelPath)
{
	m_entityManager = EntityManager();
	loadLevel(levelPath);

	m_editorText.setCharacterSize(24);
	registerAction(sf::Keyboard::W, "MOVE_UP");  
	registerAction(sf::Keyboard::A, "MOVE_LEFT");
	registerAction(sf::Keyboard::S, "MOVE_DOWN");
	registerAction(sf::Keyboard::D, "MOVE_RIGHT");
	registerAction(sf::Keyboard::P, "PLAY");
	registerAction(sf::Keyboard::Space, "SAVE");
	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
	registerAction(sf::Keyboard::G, "TOGGLE_GRID");
	
	registerMouseButtonAction(sf::Mouse::Left, "SELECT");
	registerMouseButtonAction(sf::Mouse::Right, "DELETE");
	registerMouseWheelAction(0, "SCROLL UP");
	registerMouseWheelAction(1, "SCROLL DOWN");

	m_editorText.setFont(m_game->assets().getFont("PiecesOfEight"));

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
	




	//Scrollable Pallet Strings 
	std::vector<std::string> roads = {"Dec","HRoadL", "HRoadM", "HRoadR", "VRoadT", "VRoadM", "VRoadB", "TurnBL" , "TurnBR", "TurnTL", "TurnTR" };
	std::vector<std::string> lands = { "Land", "GrassMM","LandGrass", "CliffMM","LandGrass2","LandRock","Nimbus" };
	std::vector<std::string> cliffs = { "Cliff","GrassCliffBM", "GrassCliffBL",  "GrassCliffBR", "GrassCliffMR", "GrassCliffML", "GrassCliffTR", "GrassCliffTM", "GrassCliffTL" };
	std::vector<std::string> waters = { "Water", "GWaterTL", "GWaterTM", "GWaterTR", "GWaterML", "GWaterMM", "GWaterMR", "GWaterBL", "GWaterBM", "GWaterBR" };
	std::vector<std::string> enemies = { "NPC", "Archer", "Slime", "Skull", "Tree" };
	std::vector<std::string> special = { "Tile", "GravityWell", "Spikes", "RCrystal", "BCrystal","Rock", "WoodenBox","Raft","Nimbus","Town", "Castle"};
	std::vector<std::string> decs = { "Dec", "Flower", "Flower2", "Branch", "Branch2"};
	std::vector<std::string> items = { "Item", "Arrow", "HealthPotion", "StrengthPotion", "SpeedPotion", "PoisonFlask" };
	std::vector<std::string> player = { "Player", "StandDown" };

	size_t xi = 32;

	
	m_palletNames.push_back(lands);
	m_palletNames.push_back(cliffs);
	m_palletNames.push_back(waters);
	m_palletNames.push_back(roads);
	m_palletNames.push_back(decs);
	m_palletNames.push_back(special);
	m_palletNames.push_back(items);
	m_palletNames.push_back(enemies);
	m_palletNames.push_back(player);



	//Building the Pallet
	for (std::vector<std::string> palletData : m_palletNames)
	{
		std::string defaultTag = palletData[0];
		palletData.erase(palletData.begin());
		std::string animation = palletData[0];
		int animationIndex = palletData.size();
		auto& e = m_entityManager.addEntity("pallet");
		e->addComponent<CTransform>(Vec2(xi, 32));
		e->addComponent<CAnimation>(m_game->assets().getAnimation(animation), true);
		e->addComponent<CBoundingBox>(Vec2(64,64),true, true);
		e->addComponent<CPalletData>(palletData, defaultTag, animationIndex);
		auto& entityConfig = e->addComponent<CEntityConfig>();
		entityConfig.tag = defaultTag;
		if (entityConfig.tag == "Tile")
		{
			entityConfig.vision = true;
			entityConfig.movement = true;
			entityConfig.animation = animation;
			entityConfig.attribute = "Physical";
			entityConfig.sizeWidth = 64;
			entityConfig.sizeHeight = 64;

		}
		else if (entityConfig.tag == "Water")
		{
			entityConfig.tag = "Water";
			entityConfig.vision = false;
			entityConfig.movement = true;
			entityConfig.animation = animation;
			entityConfig.attribute = "Land";
			entityConfig.sizeWidth = 64;
			entityConfig.sizeHeight = 64;

		}
		else if (entityConfig.tag == "Cliff")
		{
			entityConfig.tag = "Land";
			entityConfig.vision = false;
			entityConfig.movement = true;
			entityConfig.animation = animation;
			entityConfig.attribute = "Land";
			entityConfig.sizeWidth = 64;
			entityConfig.sizeHeight = 64;

		}
		else if (entityConfig.tag == "Land")
		{
			entityConfig.vision = false;
			entityConfig.movement = false;
			entityConfig.animation = animation;
			entityConfig.attribute = "Land";
			entityConfig.sizeWidth = 64;
			entityConfig.sizeHeight = 64;
		}
		else if (entityConfig.tag == "Dec")
		{
			entityConfig.tag = "Dec";
			entityConfig.vision = false;
			entityConfig.movement = false;
			entityConfig.animation = animation;
			entityConfig.attribute = "Passive";
			entityConfig.sizeWidth = 64;
			entityConfig.sizeHeight = 64;
		}
		else if (entityConfig.tag == "NPC")
		{
			entityConfig.vision = false;
			entityConfig.movement = true;
			entityConfig.ai = "Follow";
			entityConfig.damage = 1;
			entityConfig.speed = 4;
			entityConfig.health = 3;
			entityConfig.animation = animation;
			entityConfig.attribute = "Physical";
			entityConfig.sizeWidth = 64;
			entityConfig.sizeHeight = 64;
		}
		else if (entityConfig.tag == "Player")
		{
			entityConfig.vision = false;
			entityConfig.movement = true;
			entityConfig.speed = 5;
			entityConfig.health = 3;
			entityConfig.sizeWidth = 48;
			entityConfig.sizeHeight = 48;
			entityConfig.attribute = "Physical";

		}
		else if (entityConfig.tag == "Item")
		{
			entityConfig.tag = "Tile";
			entityConfig.vision = false;
			entityConfig.movement = false;
			entityConfig.animation = animation;
			entityConfig.attribute = "Physical";
			entityConfig.sizeWidth = 64;
			entityConfig.sizeHeight = 64;
		}


		xi += 64;
	}
	//Prepareing cursor entity
	m_selected = m_entityManager.addEntity("cursor");
	auto& selectedEntityConfig = m_selected->addComponent<CEntityConfig>();
	//Prepare buttons
	auto& button = m_entityManager.addEntity("button");
	button->addComponent<CButtonToggle>(&m_dark);
	button = m_entityManager.addEntity("button");
	button->addComponent<CButtonNumCycle>(&selectedEntityConfig.sizeWidth, 64);
	button = m_entityManager.addEntity("button");
	button->addComponent<CButtonNumCycle>(&selectedEntityConfig.sizeHeight, 64);
	button = m_entityManager.addEntity("button");
	button->addComponent<CButtonToggle>(&selectedEntityConfig.vision);
	button = m_entityManager.addEntity("button");
	button->addComponent<CButtonToggle>(&selectedEntityConfig.movement);
	button = m_entityManager.addEntity("button");
	button->addComponent<CButtonToggle>(&selectedEntityConfig.isMovingTile);
	button = m_entityManager.addEntity("button");
	button->addComponent<CButtonToggle>(&selectedEntityConfig.isLightSource);
	button = m_entityManager.addEntity("button");
	std::vector<std::string> opts = { "Follow","Patrol","Mimic" , "Stand" };
	button->addComponent<CButtonStringCycle>(&selectedEntityConfig.ai, opts, 0);
	button = m_entityManager.addEntity("button");
	button->addComponent<CButtonToggle>(&selectedEntityConfig.isRange);
	button = m_entityManager.addEntity("button");
	button->addComponent<CButtonNumCycle>(&selectedEntityConfig.range, 15);
	button = m_entityManager.addEntity("button");
	button->addComponent<CButtonNumCycle>(&selectedEntityConfig.speed, 15);
	button = m_entityManager.addEntity("button");
	button->addComponent<CButtonSpecial>();
	button = m_entityManager.addEntity("button");
	button->addComponent<CButtonNumCycle>(&selectedEntityConfig.pointX, 19);
	button = m_entityManager.addEntity("button");
	button->addComponent<CButtonNumCycle>(&selectedEntityConfig.pointY, 11);
	button = m_entityManager.addEntity("button");
	button->addComponent<CButtonNumCycle>(&selectedEntityConfig.damage, 15);
	button = m_entityManager.addEntity("button");
	button->addComponent<CButtonNumCycle>(&selectedEntityConfig.health, 30);
	button = m_entityManager.addEntity("button");
	button->addComponent<CButtonToggle>(&selectedEntityConfig.boss);
	m_entityManager.update();
	auto& buttons = m_entityManager.getEntities("button");

	
	//building on screen text, options window
	m_palletRect.setPosition(sf::Vector2f(13, 13));
	m_palletRect.setSize(sf::Vector2f(xi - 32, 64));
	m_palletRect.setFillColor(sf::Color::Magenta);
	m_palletRect.setOutlineColor(sf::Color::Black);
	m_palletRect.setOutlineThickness(13);

	m_optionsBox.setPosition(sf::Vector2f(1014, 74));
	m_optionsBox.setSize(sf::Vector2f(256, 480));
	m_optionsBox.setFillColor(sf::Color(211, 211, 211,190));
	m_optionsBox.setOutlineThickness(4);
	m_optionsBox.setOutlineColor(sf::Color::Black);
	sf::Text t = sf::Text();
	t.setFont(m_game->assets().getFont("Computer"));
	t.setCharacterSize(15);
	t.setFillColor(sf::Color::Black);
	size_t y = 79;
	const size_t x = 1028;
	for (size_t i=0; i<m_componentTitles.size();i++)
	{
		t.setPosition(sf::Vector2f(x, y));
		t.setString(m_componentTitles[i]);
		m_entityConfigDisplay.push_back(t);
		if (i < buttons.size()) 
		{
			auto& button = buttons[i];
			if (button->hasComponent<CButtonSpecial>())
			{
				button->addComponent<CAnimation>(m_game->assets().getAnimation("PlusButton"), true);
			}
			else
			{
				button->addComponent<CAnimation>(m_game->assets().getAnimation("Button"), true);
			}
			button->addComponent<CBoundingBox>(Vec2(20, 20));
			auto& buttonTC = button->addComponent<CTransform>();
			buttonTC.pos = Vec2(1255, y);
			if(i > 12)
			{
				buttonTC.pos = Vec2(1255, y + 46);
			}
			else if (i > 0)
			{
				buttonTC.pos = Vec2(1255, y+23);
			}
			
		}
		
		y += 23;
	}
	//Editor Instructions
	m_editorText.setFillColor(sf::Color(0, 0, 0));
	m_editorText.setString("Navigate World <WASD>   SAVE <SPACEBAR>   SAVE PLAY <P>   BACK <Esc> Cycle Items <SCROLL>");
}

void Scene_Editor::loadLevel(const std::string& filename)
{
	

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
		
		if (line == "Land")
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

			auto t = m_entityManager.addEntity("Land");
			iss >> animation >> roomX >> roomY >> tileX >> tileY >> sizeX >> sizeY >>  blockMove >> blockVis;
			t->addComponent<CAnimation>(m_game->assets().getAnimation(animation), true);
			t->addComponent<CTransform>(getPosition(roomX, roomY, tileX, tileY));
			t->addComponent<CBoundingBox>(Vec2(64, 64), true, false);
			auto& entityConfig = t->addComponent<CEntityConfig>();
			entityConfig.pos = getPosition(roomX, roomY, tileX, tileY);
			entityConfig.animation = animation;
			entityConfig.sizeWidth = sizeX;
			entityConfig.sizeHeight = sizeY;
			entityConfig.vision = blockVis;
			entityConfig.movement = blockMove;
			entityConfig.tag = "Land";
			entityConfig.roomX = roomX;
			entityConfig.roomY = roomY;
			entityConfig.tileX = tileX;
			entityConfig.tileY = tileY;
			entityConfig.attribute = "Land";

		}
		else if (line == "Water")
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

			auto t = m_entityManager.addEntity("Water");
			iss >> animation >> roomX >> roomY >> tileX >> tileY >> sizeX >> sizeY >> blockMove >> blockVis;
			t->addComponent<CAnimation>(m_game->assets().getAnimation(animation), true);
			t->addComponent<CTransform>(getPosition(roomX, roomY, tileX, tileY));
			t->addComponent<CBoundingBox>(Vec2(64, 64), true, false);
			auto& entityConfig = t->addComponent<CEntityConfig>();
			entityConfig.pos = getPosition(roomX, roomY, tileX, tileY);
			entityConfig.animation = animation;
			entityConfig.sizeWidth = sizeX;
			entityConfig.sizeHeight = sizeY;
			entityConfig.vision = blockVis;
			entityConfig.movement = blockMove;
			entityConfig.tag = "Water";
			entityConfig.roomX = roomX;
			entityConfig.roomY = roomY;
			entityConfig.tileX = tileX;
			entityConfig.tileY = tileY;
			entityConfig.attribute = "Land";

		}
		else if (line == "MovingTile")
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

			auto t = m_entityManager.addEntity("MovingTile");
			iss >> animation >> roomX >> roomY >> tileX >> tileY >> sizeX >> sizeY >> speed >> numPatrol;
			t->addComponent<CAnimation>(m_game->assets().getAnimation(animation), true);
			t->addComponent<CTransform>(getPosition(roomX, roomY, tileX, tileY));
			t->addComponent<CBoundingBox>(Vec2(64, 64), true, false);
			auto& entityConfig = t->addComponent<CEntityConfig>();
			entityConfig.pos = getPosition(roomX, roomY, tileX, tileY);
			entityConfig.animation = animation;
			entityConfig.sizeWidth = sizeX;
			entityConfig.sizeHeight = sizeY;
			entityConfig.vision = false;
			entityConfig.movement = false;
			entityConfig.tag = "MovingTile";
			entityConfig.roomX = roomX;
			entityConfig.roomY = roomY;
			entityConfig.tileX = tileX;
			entityConfig.tileY = tileY;
			entityConfig.speed = speed;
			entityConfig.attribute = "Physical";
			entityConfig.isMovingTile = true;
			for (int i = 0; i < numPatrol; i++)
			{
				int x, y;
				iss >> x >> y;
				patrolCoords.push_back(Vec2(x, y));
				entityConfig.patrol.push_back(Vec2(x, y));
			}

		}
		else if (line == "Tile")
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

			auto t = m_entityManager.addEntity("Tile");
			iss >> animation >> roomX >> roomY >> tileX >> tileY >> sizeX >> sizeY >> blockMove >> blockVis;
			t->addComponent<CAnimation>(m_game->assets().getAnimation(animation), true);
			t->addComponent<CTransform>(getPosition(roomX, roomY, tileX, tileY));
			t->addComponent<CBoundingBox>(Vec2(64,64), true, false);
			auto& entityConfig = t->addComponent<CEntityConfig>();
			entityConfig.pos = getPosition(roomX, roomY, tileX, tileY);
			entityConfig.animation = animation;
			entityConfig.sizeWidth = sizeX;
			entityConfig.sizeHeight = sizeY;
			entityConfig.vision = blockVis;
			entityConfig.movement = blockMove;
			entityConfig.tag = "Tile";
			entityConfig.roomX = roomX;
			entityConfig.roomY = roomY;
			entityConfig.tileX = tileX;
			entityConfig.tileY = tileY;
			entityConfig.attribute = "Physical";

		}

		else if (line == "Dec")
		{
			std::string animation;
			int roomX, roomY, tileX, tileY;
			int sizeX;
			int sizeY;

			auto d = m_entityManager.addEntity("Dec");
			iss >> animation >> roomX >> roomY >> tileX >> tileY >> sizeX >> sizeY;
			d->addComponent<CAnimation>(m_game->assets().getAnimation(animation), true);
			d->addComponent<CTransform>(getPosition(roomX, roomY, tileX, tileY));
			d->addComponent<CBoundingBox>(Vec2(64, 64), true, false);
			auto& entityConfig = d->addComponent<CEntityConfig>();
			entityConfig.pos = getPosition(roomX, roomY, tileX, tileY);
			entityConfig.animation = animation;
			entityConfig.sizeWidth = sizeX;
			entityConfig.sizeHeight = sizeY;
			entityConfig.roomX = roomX;
			entityConfig.roomY = roomY;
			entityConfig.tileX = tileX;
			entityConfig.tileY = tileY;
			entityConfig.tag = "Dec";
			entityConfig.attribute = "Passive";
		}

		else if (line == "Player")
		{
			m_player = true;
			float spawnX, spawnY, characterWidth, characterHeight, maxHealth;
			float speed;

			iss >> spawnX >> spawnY >> characterWidth >> characterHeight >> speed >> maxHealth;

			auto& entity = m_entityManager.addEntity("Player");
			entity->addComponent<CTransform>(Vec2(spawnX, spawnY), Vec2(speed,speed), Vec2(1,1), 0.0f);
			entity->addComponent<CAnimation>(m_game->assets().getAnimation("StandDown"), true);
			entity->addComponent<CBoundingBox>(Vec2(64, 64), true, false);
			auto& entityConfig = entity->addComponent<CEntityConfig>();
			entityConfig.pos = Vec2(spawnX, spawnY);
			entityConfig.animation = "StandDown";
			entityConfig.sizeWidth = characterWidth;
			entityConfig.sizeHeight = characterHeight;
			entityConfig.health = maxHealth;
			entityConfig.speed = speed;
			entityConfig.tag = "Player";
			entityConfig.attribute = "Physical";
		}
		else if (line == "NPC")
		{
			m_npc = true;
			std::string animation, behavior;
			int roomX, roomY, tileX, tileY, maxHealth, damage;
			int sizeX;
			int sizeY;
			bool blockMove, blockVis;
			float speed;
			iss >> animation >> roomX >> roomY >> tileX >> tileY >> sizeX >> sizeY >> blockMove >> blockVis >> maxHealth >> damage >> behavior >> speed;
			auto n = m_entityManager.addEntity("NPC");
			auto& entityConfig = n->addComponent<CEntityConfig>();
			if (behavior == "Range")
			{
				entityConfig.isRange = true;
				entityConfig.range = speed;
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
					entityConfig.patrol.push_back(Vec2(x,y));
					entityConfig.ai = "Patrol";
				}

			}
			
			n->addComponent<CAnimation>(m_game->assets().getAnimation(animation), true);
			n->addComponent<CTransform>(getPosition(roomX, roomY, tileX, tileY));
			n->addComponent<CBoundingBox>(Vec2(64,64),1, 0);
			
			entityConfig.pos = getPosition(roomX, roomY, tileX, tileY);
			entityConfig.animation = animation;
			entityConfig.vision = blockVis;
			entityConfig.movement = blockMove;
			entityConfig.tag = "NPC";
			entityConfig.roomX = roomX;
			entityConfig.roomY = roomY;
			entityConfig.tileX = tileX;
			entityConfig.tileY = tileY;
			entityConfig.damage = damage;
			entityConfig.health = maxHealth;
			entityConfig.attribute = "Physical";
			
			entityConfig.sizeWidth = sizeX;
			entityConfig.sizeHeight = sizeY;
			entityConfig.speed = speed;
			entityConfig.ai = behavior;
			
		}
		else if (line == "Dark")
		{
			m_dark = true;
		}
		else if (line == "LightSource")
		{
			std::string animation;
			int roomX;
			int roomY;
			int tileX;
			int tileY;
			int sizeX;
			int sizeY;
			bool blockMove;
	

			auto t = m_entityManager.addEntity("LightSource");
			iss >> animation >> roomX >> roomY >> tileX >> tileY >> sizeX >> sizeY >> blockMove;
			t->addComponent<CAnimation>(m_game->assets().getAnimation(animation), true);
			t->addComponent<CTransform>(getPosition(roomX, roomY, tileX, tileY));
			t->addComponent<CBoundingBox>(Vec2(64, 64), true, false);
			auto& entityConfig = t->addComponent<CEntityConfig>();
			entityConfig.pos = getPosition(roomX, roomY, tileX, tileY);
			entityConfig.animation = animation;
			entityConfig.sizeWidth = sizeX;
			entityConfig.sizeHeight = sizeY;
			entityConfig.movement = blockMove;
			entityConfig.tag = "LightSource";
			entityConfig.roomX = roomX;
			entityConfig.roomY = roomY;
			entityConfig.tileX = tileX;
			entityConfig.tileY = tileY;
			entityConfig.isLightSource = true;
			entityConfig.attribute = "Physical";
		}
	}

}

void Scene_Editor::saveLevel(const std::string& filename, EntityManager& entityManager)
{
	std::ofstream file;
	file.open(filename);
	if (file.is_open())
	{
		if (m_dark)
		{
			file << "Dark" << "\n";
		}
		for (auto& e : entityManager.getEntities())
		{
			std::string tag = e->tag();
			if (tag != "pallet" && tag != "cursor" && tag != "button")
			{
				auto& entityConfig = e->getComponent<CEntityConfig>();
				auto& transform = e->getComponent<CTransform>();
				if (transform.pos.x < 0)
				{
					entityConfig.roomX = (transform.pos.x-1280)/1280;
				}
				else
				{
					entityConfig.roomX = std::trunc(transform.pos.x / 1280);
				}
				if (transform.pos.y < 0)
				{
					entityConfig.roomY = (transform.pos.y - 768) / 768;
				}
				else
				{
					entityConfig.roomY = std::trunc(transform.pos.y / 768);
				}
				entityConfig.tileX = (transform.pos.x - entityConfig.roomX * 1280) / 64;
				entityConfig.tileY = (transform.pos.y - entityConfig.roomY * 768) / 64;
				if (tag == "Land")
				{
					file << tag << " " << entityConfig.animation << " " << entityConfig.roomX << " " << entityConfig.roomY
						<< " " << entityConfig.tileX << " " << entityConfig.tileY << " " << entityConfig.sizeWidth << " " << entityConfig.sizeHeight<<
						" "<< entityConfig.movement << " " << entityConfig.vision << "\n";
				}
				else if (tag == "Water")
				{
					file << tag << " " << entityConfig.animation << " " << entityConfig.roomX << " " << entityConfig.roomY
						<< " " << entityConfig.tileX << " " << entityConfig.tileY << " " << entityConfig.sizeWidth << " " << entityConfig.sizeHeight <<
						" " << entityConfig.movement << " " << entityConfig.vision << "\n";
				}
				else if (tag == "Dec")
				{
					file << tag << " " << entityConfig.animation << " " << entityConfig.roomX << " " << entityConfig.roomY
						<< " " << entityConfig.tileX << " " << entityConfig.tileY << " " << entityConfig.sizeWidth << " " << entityConfig.sizeHeight<<
						" "<< entityConfig.movement << " " << entityConfig.vision << "\n";
				}
				else if (tag == "Tile")
				{
					file << tag << " " << entityConfig.animation << " " << entityConfig.roomX << " " << entityConfig.roomY
						<< " " << entityConfig.tileX << " " << entityConfig.tileY << " " << entityConfig.sizeWidth << " " << entityConfig.sizeHeight<<
						" "<< entityConfig.movement << " " << entityConfig.vision << "\n";
				}
				else if (tag == "MovingTile")
				{
					file << tag << " " << entityConfig.animation << " " << entityConfig.roomX << " " << entityConfig.roomY
						<< " " << entityConfig.tileX << " " << entityConfig.tileY << " " << entityConfig.sizeWidth << " " << entityConfig.sizeHeight<<
						" " << entityConfig.speed<<" "<<entityConfig.patrol.size();
					for (Vec2 point : entityConfig.patrol)
					{
						file << " " << point.x << " " << point.y;
					}
					file << "\n";
				}
				else if (tag == "LightSource")
				{
					file << tag << " " << entityConfig.animation << " " << entityConfig.roomX << " " << entityConfig.roomY
						<< " " << entityConfig.tileX << " " <<entityConfig.tileY<<" " <<entityConfig.sizeWidth << " " << entityConfig.sizeHeight << 
						" "<<entityConfig.movement << "\n";
				}
				else if (tag == "NPC")
				{
					file << tag << " " << entityConfig.animation << " " << entityConfig.roomX << " " << entityConfig.roomY
						<< " " << entityConfig.tileX << " " << entityConfig.tileY << " " << entityConfig.sizeWidth << " " << entityConfig.sizeHeight<<
						" "<< entityConfig.movement << " " << entityConfig.vision <<
						" " << entityConfig.health << " " << entityConfig.damage << " " ; 
					if (entityConfig.isRange)
					{
						file << "Range" << " " << entityConfig.range << " ";
					}
					file << entityConfig.ai << " " << entityConfig.speed;
					if (entityConfig.ai == "Patrol")
					{
						file << " " << entityConfig.patrol.size();
						for (Vec2 point : entityConfig.patrol)
						{
							file << " " << point.x << " " << point.y;
						}
					}	
					file << "\n";
				}
				else if (tag == "Player")
				{
					file << tag << " "<<transform.pos.x<< " "<< transform.pos.y<< " "<<entityConfig.sizeWidth<< " "<< entityConfig.sizeHeight<< " "
						<< entityConfig.speed <<" "<< entityConfig.health <<"\n";
				}

			}
		}
		file.close();
	}
}

Vec2 Scene_Editor::getPosition(int rx, int ry, int tx, int ty) const
{
	return Vec2((rx * 1280) + ((tx + 0.5) * 64), (ry * 768) + ((ty + 0.5) * 64));
}

void Scene_Editor::update()
{
	m_entityManager.update();
	sMovement();
	sCollision();
	sCamera();
	if (m_toSave)
	{
		saveLevel(m_levelPath, m_entityManager);
		m_toSave = false;
	}
	if (m_toPlay && m_player && m_npc)
	{
		m_game->changeScene("ZELDA", std::make_shared<Scene_Zelda>(m_game, m_levelPath));
		m_toPlay = false;
	}
	m_toPlay = false;
	//Update Entity Options Window
	updateEntityConfigDisplay();
	m_currentFrame++;

}
void Scene_Editor::sCollision()
{
	
	if (m_click)
	{
		bool isPallet = false;
		bool isButton = false;
		bool sameCollision = false;
		std::vector<std::shared_ptr<Entity>> toBeDeleted = {};
		std::shared_ptr<Entity> toCopy;
		for (auto& e : m_entityManager.getEntities())
		{
			if (e->hasComponent<CBoundingBox>())
			{		
				if (e != m_selected && (Physics::IsInside(Vec2(sf::Mouse::getPosition(m_game->window()).x  + m_xSteps * 64 , sf::Mouse::getPosition(m_game->window()).y  + m_ySteps * 64 ), e)))
				{
					
					if (m_leftClick)
					{
						//If Clicked on Pallet , changed selected entity to pallet entity
						if (e->tag() == "pallet")
						{
							isPallet = true;
							std::string animation = e->getComponent<CAnimation>().animation.getName();
							m_selected->addComponent<CTransform>(Vec2(sf::Mouse::getPosition(m_game->window()).x, sf::Mouse::getPosition(m_game->window()).y));
							m_selected->addComponent<CAnimation>(m_game->assets().getAnimation(animation), true);
							m_selected->getComponent<CEntityConfig>() = e->getComponent<CEntityConfig>();
							m_selected->addComponent<CBoundingBox>() = e->getComponent<CBoundingBox>();
							m_selected->addComponent<CPalletData>() = e->getComponent<CPalletData>();

							break;
						}
						else if (e->tag() == "button")
						{
							isButton = true;
							if (e->hasComponent<CButtonToggle>())
							{
								bool previousMove = m_selected->getComponent<CEntityConfig>().isMovingTile;
								bool previousLight = m_selected->getComponent<CEntityConfig>().isLightSource;
								auto& btnTog = e->getComponent<CButtonToggle>();
								*btnTog.data = !*btnTog.data;
								if (m_selected->getComponent<CEntityConfig>().tag == "Tile" || m_selected->getComponent<CEntityConfig>().tag == "MovingTile" || m_selected->getComponent<CEntityConfig>().tag == "LightSource")
								{
									if (!m_selected->getComponent<CEntityConfig>().isMovingTile && !m_selected->getComponent<CEntityConfig>().isLightSource)
									{
										m_selected->getComponent<CEntityConfig>().tag = "Tile";
									}
									else if (m_selected->getComponent<CEntityConfig>().isMovingTile && !previousMove)
									{
										m_selected->getComponent<CEntityConfig>().tag = "MovingTile";
										m_selected->getComponent<CEntityConfig>().isLightSource = false;
										m_selected->getComponent<CEntityConfig>().movement = false;
										m_selected->getComponent<CEntityConfig>().vision = false;
									}
									else if (m_selected->getComponent<CEntityConfig>().isLightSource && !previousLight)
									{
										m_selected->getComponent<CEntityConfig>().tag = "LightSource";
										m_selected->getComponent<CEntityConfig>().isMovingTile = false;
										m_selected->getComponent<CEntityConfig>().vision = false;
									}
		
								}
								else
								{
									m_selected->getComponent<CEntityConfig>().isMovingTile = false;
									m_selected->getComponent<CEntityConfig>().isLightSource = false;
								}
								
							}
							else if (e->hasComponent<CButtonNumCycle>())
							{
								auto& btnInt = e->getComponent<CButtonNumCycle>();
								if (*btnInt.data < btnInt.max)
								{
									*btnInt.data += 1;
								}
								else
								{
									*btnInt.data = 0;
								}
							}
							else if (e->hasComponent<CButtonStringCycle>())
							{

								auto& btnString = e->getComponent<CButtonStringCycle>();
								auto temp = *btnString.data;
								btnString.index++;
								if (btnString.index >= btnString.options.size())
								{
									btnString.index = 0;
								}
						
								*btnString.data = btnString.options[btnString.index];
								if (temp == *btnString.data)
								{
									btnString.index++;
									if (btnString.index >= btnString.options.size())
									{
										btnString.index = 0;
									}
									*btnString.data = btnString.options[btnString.index];
								}
							}
							else if (e->hasComponent<CButtonSpecial>())
							{
								m_selected->getComponent<CEntityConfig>().patrol.push_back(Vec2(m_selected->getComponent<CEntityConfig>().pointX, m_selected->getComponent<CEntityConfig>().pointY));
							}
							break;
						}
						//Copy clicked on entity of the same type
						else if (e->getComponent<CEntityConfig>().attribute == m_selected->getComponent<CEntityConfig>().attribute)
						{
							sameCollision = true;
							toCopy = e;
						}

					}
					//Right click clear the Cell
					else
					{
						if (e->tag() == "button")
						{
							isButton = true;
							if (e->hasComponent<CButtonToggle>())
							{
								bool previousMove = m_selected->getComponent<CEntityConfig>().isMovingTile;
								bool previousLight = m_selected->getComponent<CEntityConfig>().isLightSource;
								auto& btnTog = e->getComponent<CButtonToggle>();
								*btnTog.data = !*btnTog.data;
								if (m_selected->getComponent<CEntityConfig>().tag == "Tile" || m_selected->getComponent<CEntityConfig>().tag == "MovingTile" || m_selected->getComponent<CEntityConfig>().tag == "LightSource")
								{
									if (!m_selected->getComponent<CEntityConfig>().isMovingTile && !m_selected->getComponent<CEntityConfig>().isLightSource)
									{
										m_selected->getComponent<CEntityConfig>().tag = "Tile";
									}
									else if (m_selected->getComponent<CEntityConfig>().isMovingTile && !previousMove)
									{
										m_selected->getComponent<CEntityConfig>().tag = "MovingTile";
										m_selected->getComponent<CEntityConfig>().isLightSource = false;
										m_selected->getComponent<CEntityConfig>().movement = false;
										m_selected->getComponent<CEntityConfig>().vision = false;
									}
									else if (m_selected->getComponent<CEntityConfig>().isLightSource && !previousLight)
									{
										m_selected->getComponent<CEntityConfig>().tag = "LightSource";
										m_selected->getComponent<CEntityConfig>().isMovingTile = false;
										m_selected->getComponent<CEntityConfig>().vision = false;
									}

								}
								else
								{
									m_selected->getComponent<CEntityConfig>().isMovingTile = false;
									m_selected->getComponent<CEntityConfig>().isLightSource = false;
								}
							}
							else if (e->hasComponent<CButtonNumCycle>())
							{
								auto& btnInt = e->getComponent<CButtonNumCycle>();
								if (*btnInt.data > 0)
								{
									*btnInt.data -= 1;
								}
								else
								{
									*btnInt.data = btnInt.max;
								}
							}
							else if (e->hasComponent<CButtonStringCycle>())
							{
								
								auto& btnString = e->getComponent<CButtonStringCycle>();
								auto temp = *btnString.data;
								btnString.index--;
								if (btnString.index < 0)
								{
									btnString.index = btnString.options.size() - 1;
								}
								*btnString.data = btnString.options[btnString.index];
								if (temp == *btnString.data)
								{
									btnString.index--;
									if (btnString.index < 0)
									{
										btnString.index = btnString.options.size() - 1;
									}
									*btnString.data = btnString.options[btnString.index];
								}
							}
							else if (e->hasComponent<CButtonSpecial>())
							{
								m_selected->getComponent<CEntityConfig>().patrol.pop_back();
							}
							break;
						}
						else if (e->tag() != "pallet" && e->tag() != "button")
						{
							if (e->tag() == "Player")
							{
								m_player = false;
							}
							if (e->tag() == "NPC") {
								if (m_entityManager.getEntities("NPC").size() == 0)
								{
									m_npc = false;
								}
								else
								{
									for (auto& e : m_entityManager.getEntities("NPC"))
									{
										m_npc = false;
										if (e->isActive()) {
											m_npc = true;
											break;
										}
									}
								}
							}
							toBeDeleted.push_back(e);
						}
					}
				}
				
			}
		}
		if (m_rightClick && !isButton)
		{
			for (auto& e : toBeDeleted)
			{
				e->destroy();
			}
		}
		if (m_leftClick && !isPallet && !isButton && sameCollision)
		{
			std::string animation = toCopy->getComponent<CAnimation>().animation.getName();
			m_selected->addComponent<CTransform>(Vec2(sf::Mouse::getPosition(m_game->window()).x, sf::Mouse::getPosition(m_game->window()).y));
			m_selected->addComponent<CAnimation>(m_game->assets().getAnimation(animation), true);
			m_selected->getComponent<CEntityConfig>() = toCopy->getComponent<CEntityConfig>();
			m_selected->addComponent<CBoundingBox>() = toCopy->getComponent<CBoundingBox>();
			m_selected->addComponent<CPalletData>() = toCopy->getComponent<CPalletData>();
		}
		//If no collison with same type entity , place entity in the empty space
		if (!isPallet && m_leftClick && m_selected->hasComponent<CTransform>() && !sameCollision && !isButton)
		{
			if (!(m_player && m_selected->getComponent<CEntityConfig>().tag == "Player"))
			{
				if (m_selected->getComponent<CEntityConfig>().tag == "Player")
				{
					m_player = true;
				}
				if (m_selected->getComponent<CEntityConfig>().tag == "NPC")
				{
					m_npc = true;
				}
				int xmodifier = 1;
				int ymodifier = 1;
				if (m_xSteps < 0 && sf::Mouse::getPosition(m_game->window()).x < m_xSteps*64*-1) 
				{
					xmodifier = -64;
				}
				else {
					xmodifier = 0;
				}
				if (m_ySteps < 0 && sf::Mouse::getPosition(m_game->window()).y < m_ySteps * 64 * -1)
				{
					ymodifier = -64;
				}
				else {
					ymodifier = 0;
				}
				auto entityConfig = m_selected->getComponent<CEntityConfig>();
				auto& entity = m_entityManager.addEntity(entityConfig.tag);
				entity->addComponent<CEntityConfig>() = entityConfig;
				entity->addComponent<CAnimation>() = m_selected->getComponent<CAnimation>();
				entity->addComponent<CTransform>() = m_selected->getComponent<CTransform>();
				entity->getComponent<CTransform>().pos.x = std::trunc(entity->getComponent<CTransform>().pos.x / 64) * 64 + 32 + xmodifier;
				entity->getComponent<CTransform>().pos.y = std::trunc(entity->getComponent<CTransform>().pos.y / 64) * 64 + 32 + ymodifier;
				entity->addComponent<CBoundingBox>() = m_selected->getComponent<CBoundingBox>();
			}
		}
	}
	m_click = false;
	m_leftClick = false;
	m_rightClick = false;
}

void Scene_Editor::sMovement()
{
	
	
	if (m_stepUp)
	{
		m_ySteps--;
		m_stepUp = false;
		
	}
	else if(m_stepDown)
	{
		m_ySteps++;
		m_stepDown = false;
		
	}
	else if(m_stepLeft)
	{
		m_xSteps--;
		m_stepLeft = false;
		
	}
	else if (m_stepRight)
	{
		m_xSteps++;
		m_stepRight = false;
		
	}

	//Move Cursor Image
	if (m_selected->hasComponent<CTransform>())
	{
		m_selected->getComponent<CTransform>().pos = Vec2(sf::Mouse::getPosition(m_game->window()).x  + m_xSteps * 64 , sf::Mouse::getPosition(m_game->window()).y  + m_ySteps * 64 );
		std::vector<std::string> animations = m_selected->getComponent<CPalletData>().animations;
		int animationIndex = m_selected->getComponent<CPalletData>().animationIndex;
		if (animations.size() != 0) {
			m_selected->getComponent<CAnimation>().animation = m_game->assets().getAnimation(animations[animationIndex % animations.size()]);
			m_selected->getComponent<CEntityConfig>().animation = m_selected->getComponent<CAnimation>().animation.getName();
		}
		
	}

	//Keep HUD following camera
	float x = 32;
	m_palletRect.setPosition(sf::Vector2f(m_xSteps * m_gridSize.x + 13, m_ySteps * m_gridSize.y + 13));
	for (auto& e : m_entityManager.getEntities("pallet"))
	{
		e->getComponent<CTransform>().pos = Vec2(x + m_xSteps * m_gridSize.x + 13, 32 + m_ySteps * m_gridSize.y + 13);
		x += 64;
	}
	m_editorText.setPosition(sf::Vector2f(0 + m_xSteps * m_gridSize.x + 32, m_game->window().getSize().y - m_editorText.getCharacterSize() + m_ySteps * m_gridSize.y - 32));
	m_optionsBox.setPosition(sf::Vector2f(1014 + m_xSteps * m_gridSize.x, 74 + m_ySteps * m_gridSize.y));
	size_t y = 70;
	auto& buttons = m_entityManager.getEntities("button");
	for (size_t i=0; i<m_componentTitles.size(); i++)
	{
		auto& text = m_entityConfigDisplay[i];
		text.setPosition(sf::Vector2f(1028 + m_xSteps * m_gridSize.x, y + 10 + m_ySteps * m_gridSize.y));
		if (i > 12)
		{
			text.setPosition(sf::Vector2f(1028 + m_xSteps * m_gridSize.x, y + 33 + m_ySteps * m_gridSize.y));
		}
		if (i < buttons.size())
		{
			auto& button = buttons[i];
			auto& buttonTC = button->getComponent<CTransform>();
			buttonTC.pos = Vec2(1255 + m_xSteps * m_gridSize.x, y + 20  + m_ySteps * m_gridSize.y);
			if (i > 11)
			{
				buttonTC.pos = Vec2(1255 + m_xSteps * m_gridSize.x, y + 20 + 46 + m_ySteps * m_gridSize.y);
				
			}
			else if (i > 0)
			{
				buttonTC.pos = Vec2(1255 + m_xSteps * m_gridSize.x, y + 20 + 23  + m_ySteps * m_gridSize.y);
			}

		}
		y += 23;
	}
}

void Scene_Editor::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{

		if (action.name() == "QUIT") { onEnd(); }
		else if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
		else if (action.name() == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; }
		else if (action.name() == "MOVE_UP") 
		{ 
			m_stepUp = true;
			m_stepDown = false;
			m_stepLeft = false;
			m_stepRight = false;
		}
		else if (action.name() == "MOVE_DOWN") 
		{
			m_stepUp =false;
			m_stepDown = true;
			m_stepLeft = false;
			m_stepRight = false;
		}
		else if (action.name() == "MOVE_LEFT") 
		{
			m_stepUp = false;
			m_stepDown = false;
			m_stepLeft = true;
			m_stepRight = false;
		}
		else if (action.name() == "MOVE_RIGHT") 
		{ 
			m_stepUp = false;
			m_stepDown = false;
			m_stepLeft = false;
			m_stepRight = true;
		}
		else if (action.name() == "PLAY") 
		{ 
			m_toSave = true;
			m_toPlay = true;
		}
		else if (action.name() == "SELECT") 
		{
			m_click = true;
			m_leftClick = true;
		}
		else if (action.name() == "DELETE")
		{
			m_click = true;
			m_rightClick = true;
		}
		else if (action.name() == "SAVE")
		{
			m_toSave = true;
		}
		else if (action.name() == "SCROLL UP")
		{
			if (m_selected->hasComponent<CPalletData>())
			{
				m_selected->getComponent<CPalletData>().animationIndex++;
			}
		}
		else if (action.name() == "SCROLL DOWN")
		{
			if (m_selected->hasComponent<CPalletData>())
			{
				int& index = m_selected->getComponent<CPalletData>().animationIndex;
				if (index > 0) { index--; }
				else { index = m_selected->getComponent<CPalletData>().animations.size() - 1; }
			}
		}
	}
	else if (action.type() == "END")
	{

	}
}

void Scene_Editor::sCamera()
{
	sf::View view = m_game->window().getView();
	m_prevView = view.getCenter();
	auto windowSize = m_game->window().getSize();
	int centreX = windowSize.x / 2 + m_xSteps * m_gridSize.x;
	int centreY = windowSize.y / 2 + m_ySteps * m_gridSize.y;
	

	view.setCenter(centreX, centreY);
	
	m_game->window().setView(view);
	//Parallax Background based on camera position perspectice
	sf::Vector2f deltaVector = view.getCenter() - m_prevView;
	if (std::trunc(deltaVector.x) != 0)
	{
		m_parallaxXStep += std::trunc(deltaVector.x);
		if (std::abs(m_parallaxXStep) >= 3000)
		{
			m_parallaxXStep = 0;
		}
	}
	if (std::trunc(deltaVector.y) != 0)
	{
		m_parallaxYStep += std::trunc(deltaVector.y);
		if (std::abs(m_parallaxYStep) >= 1630)
		{
			m_parallaxYStep = 0;
		}
	}
	m_background1.setPosition(view.getCenter() - sf::Vector2f((m_game->window().getSize().x / 2 + 400) + m_parallaxXStep * 0.1, (m_game->window().getSize().y / 2 + 200) + m_parallaxYStep * 0.1));
	m_background2.setPosition(view.getCenter() - sf::Vector2f(m_game->window().getSize().x / 2 + 20, m_game->window().getSize().y / 2 + 32));
}

void Scene_Editor::onEnd()
{
	m_game->changeScene("EDITOR MENU", std::make_shared<Scene_Editor_Menu>(m_game));

}

void Scene_Editor::updateEntityConfigDisplay()
{
	const auto& entConfig = m_selected->getComponent<CEntityConfig>();
	size_t i = 0;
	m_entityConfigDisplay[i].setString(m_componentTitles[i] + std::to_string(m_dark));
	i++;
	m_entityConfigDisplay[i].setString(m_componentTitles[i]+entConfig.tag);
	i++;
	m_entityConfigDisplay[i].setString(m_componentTitles[i] + std::to_string((int)entConfig.sizeWidth));
	i++;
	m_entityConfigDisplay[i].setString(m_componentTitles[i] + std::to_string((int)entConfig.sizeHeight));
	i++;
	m_entityConfigDisplay[i].setString(m_componentTitles[i] + std::to_string(entConfig.vision));
	i++;
	m_entityConfigDisplay[i].setString(m_componentTitles[i] + std::to_string(entConfig.movement));
	i++;
	m_entityConfigDisplay[i].setString(m_componentTitles[i] + std::to_string(entConfig.isMovingTile));
	i++;
	m_entityConfigDisplay[i].setString(m_componentTitles[i] + std::to_string(entConfig.isLightSource));
	i++;
	m_entityConfigDisplay[i].setString(m_componentTitles[i] + entConfig.ai);
	i++;
	m_entityConfigDisplay[i].setString(m_componentTitles[i] + std::to_string(entConfig.isRange));
	i++;
	m_entityConfigDisplay[i].setString(m_componentTitles[i] + std::to_string((int)entConfig.range));
	i++;
	m_entityConfigDisplay[i].setString(m_componentTitles[i] + std::to_string((int)entConfig.speed));
	i++;
	m_entityConfigDisplay[i].setString(m_componentTitles[i]);
	if (entConfig.patrol.size() != 0)
	{
		for (Vec2 point : entConfig.patrol)
		{
			m_entityConfigDisplay[i].setString(m_entityConfigDisplay[i].getString() + point.toString());
		}
	}
	i++;
	m_entityConfigDisplay[i].setString(m_componentTitles[i] + std::to_string((int)entConfig.pointX));
	i++;
	m_entityConfigDisplay[i].setString(m_componentTitles[i] + std::to_string((int)entConfig.pointY));
	i++;
	m_entityConfigDisplay[i].setString(m_componentTitles[i] + std::to_string((int)entConfig.damage));
	i++;
	m_entityConfigDisplay[i].setString(m_componentTitles[i] + std::to_string((int)entConfig.health));
	i++;
	m_entityConfigDisplay[i].setString(m_componentTitles[i] + std::to_string(entConfig.boss));
}

void Scene_Editor::drawLine(const Vec2& p1, const Vec2& p2)
{
	sf::Vertex line[] = { sf::Vector2f(p1.x, p1.y), sf::Vector2f(p2.x, p2.y) };
	line[0].color = sf::Color::Black;
	line[1].color = sf::Color::Black;
	m_game->window().draw(line, 2, sf::Lines);
}

void Scene_Editor::sRender()
{
	m_game->window().clear(sf::Color(135, 206, 235));
	sf::RectangleShape tick({ 1.0f, 6.0f });
	tick.setFillColor(sf::Color::Black);
	//Draw Parallax Background
	m_game->window().draw(m_background2);
	m_game->window().draw(m_background1);
	// draw all Entity textures / animations
	if (m_drawTextures)
	{
		for (auto e : m_entityManager.getEntities("Land"))
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
		for (auto e : m_entityManager.getEntities("Water"))
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
		for (auto e : m_entityManager.getEntities("Dec"))
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
		for (auto e : m_entityManager.getEntities("Player"))
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
		for (auto e : m_entityManager.getEntities("NPC"))
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
		for (auto e : m_entityManager.getEntities("Tile"))
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
		for (auto e : m_entityManager.getEntities("MovingTile"))
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
		for (auto e : m_entityManager.getEntities("LightSource"))
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
	}
	// draw all Entity collision bounding boxes with a rectangleshape
	if (m_drawCollision)
	{
		sf::CircleShape dot(4);
		dot.setFillColor(sf::Color::Black);
		for (auto e : m_entityManager.getEntities())
		{
			if (e->tag() != "Dec" && e->tag() != "button")
			{
				auto& box = Vec2(e->getComponent<CEntityConfig>().sizeWidth, e->getComponent<CEntityConfig>().sizeHeight);
				auto& transform = e->getComponent<CTransform>();
				sf::RectangleShape rect;
				rect.setSize(sf::Vector2f(box.x - 1, box.y - 1));
				rect.setOrigin(sf::Vector2f(box.x/2, box.y/2));
				rect.setPosition(transform.pos.x, transform.pos.y);
				rect.setFillColor(sf::Color(0, 0, 0, 0));

				if (e->getComponent<CEntityConfig>().movement && e->getComponent<CEntityConfig>().vision) { rect.setOutlineColor(sf::Color::Black); }
				if (e->getComponent<CEntityConfig>().movement && !e->getComponent<CEntityConfig>().vision) { rect.setOutlineColor(sf::Color::Blue); }
				if (!e->getComponent<CEntityConfig>().movement && e->getComponent<CEntityConfig>().vision) { rect.setOutlineColor(sf::Color::Red); }
				if (!e->getComponent<CEntityConfig>().movement && !e->getComponent<CEntityConfig>().vision) { rect.setOutlineColor(sf::Color::White); }
				rect.setOutlineThickness(1);
				m_game->window().draw(rect);
			}

			if (e->getComponent<CEntityConfig>().patrol.size() != 0)
			{
				int roomX = 0;
				int roomY = 0;
				const auto& transform = e->getComponent<CTransform>();
				if (transform.pos.x < 0)
				{
					roomX = (transform.pos.x - 1280) / 1280;
				}
				else
				{
					roomX = std::trunc(transform.pos.x / 1280);
				}
				if (transform.pos.y < 0)
				{
					roomY = (transform.pos.y - 768) / 768;
				}
				else
				{
					roomY = std::trunc(transform.pos.y / 768);
				}
				auto& patrol = e->getComponent<CEntityConfig>().patrol;
				for (size_t p = 0; p < patrol.size(); p++)
				{
					dot.setPosition(roomX * 1280 + patrol[p].x * 64 + 32, roomY * 768 + patrol[p].y*64 + 32);
					m_game->window().draw(dot);
				}
			}

		}
	}
	if (m_drawGrid)
	{	
		
		for (size_t x = 0 ; x < width(); x += m_gridSize.x)
		{
			drawLine(Vec2(x + m_xSteps*m_gridSize.x, 0 + m_ySteps*m_gridSize.y), Vec2(x + m_xSteps * m_gridSize.x, height() + m_ySteps * m_gridSize.y));
		}

		for (size_t y = 0; y < height(); y += m_gridSize.y)
		{
			drawLine(Vec2(0 + m_xSteps * m_gridSize.x, y + m_ySteps * m_gridSize.y), Vec2(width() + m_xSteps * m_gridSize.x, y + m_ySteps * m_gridSize.y));
		}
	}
	m_game->window().draw(m_editorText);
	m_game->window().draw(m_palletRect);
	for (auto e : m_entityManager.getEntities("pallet"))
	{
		if (e->hasComponent<CTransform>())
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
	}
	if (m_selected->hasComponent<CAnimation>())
	{
		auto& transform = m_selected->getComponent<CTransform>();
		sf::Color c = sf::Color::White;
		auto& animation = m_selected->getComponent<CAnimation>().animation;
		animation.getSprite().setRotation(transform.angle);
		animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
		animation.getSprite().setScale(transform.scale.x, transform.scale.y);
		animation.getSprite().setColor(c);
		m_game->window().draw(animation.getSprite());
	}
	m_game->window().draw(m_optionsBox);
	for (auto& e : m_entityManager.getEntities("button"))
	{
		auto& transform = e->getComponent<CTransform>();

		if (e->hasComponent<CAnimation>())
		{
			sf::Color c = sf::Color::White;
			auto& animation = e->getComponent<CAnimation>().animation;
			animation.getSprite().setRotation(transform.angle);
			animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
			animation.getSprite().setScale(transform.scale.x, transform.scale.y);
			animation.getSprite().setColor(c);
			m_game->window().draw(animation.getSprite());
		}
	}
	for (sf::Text text : m_entityConfigDisplay)
	{
		m_game->window().draw(text);
	}
	
}


