#include "Game.h"
#include "Common.h"
#include <iostream>

Game::Game(const std::string& config) {

	init(config);
}

void Game::init(const std::string& path)
{	// Where we read the config file 
	//Todo: read in config file here 
	// use the premade PlayerConfig, EnemyConfig, BulletConfig variables 

	// set up default window parameters 
	m_window.create(sf::VideoMode(1280, 720), "Assignment 2");
	m_window.setFramerateLimit(60);

	ImGui::SFML::Init(m_window);

	// scale the imgui ui and text size by 2 
	ImGui::GetStyle().ScaleAllSizes(2.0f);
	ImGui::GetI0().FontGlobalScalee = 2.0f; 

	spawnPlayer();

}


std::shared_ptr<Entity> Game::player()
{
	auto& players = m_entities.getEntities("player");
	return players.front();
}

void Game::run()
{
	// ToDO: add pause functionality in here 
	// Some systems should function while paused (rendering) 
	// Some systesm shouldn't movement / input 

	while (m_running)
	{
		// update the entity manager 
		m_entities.update();

		// required update call to imgui 
		ImGui::SFML::Update(m_window, m_deltaClock.restart());

		sEnemySpawner();
		sMovement();
		sCollision();
		sUserInput();
		sGUI();
		sRender();


		
		m_currentFrame++; 



	}






}

void Game::spawnPlayer()
{
	// TODO: Finish adding all properties of the player with the correct values from the config

	// We create every entity by calling EntityManager.addEntity(tag)
	// This returns a std::shared_ptr<Entity>, so we use 'auto' to save typing
	auto entity = m_entities.addEntity("player");

	// Give this entity a Transform so it spawns at (200,200) with velocity (1,1) and angle 0.0f
	entity->add<CTransform>(Vec2f(200.0f, 200.0f), Vec2f(1.0f, 1.0f), 0.0f);

	// The entity's shape will have radius 32, 8 sides, dark grey fill, and red outline of thickness 4
	entity->add<CShape>(32.0f, 8, sf::Color(10, 10, 10), sf::Color(255, 0, 0), 4.0f);

	// Add an input component to the player so that we can use inputs
	entity->add<CInput>();
}


// spawn an enemy at a random position 
void Game::spawnEnemy()
{
	// Todo: make sure the enemy is spawned properly with the m_enemyConfig variables 
	// the enemy must be spawned completely within the bounds of the window 

	// record when the most recent enemy was spawned 

	m_lastEnemySpawnTime = m_currentFrame; 
}

// spawns the small enemies when a big one (input entity e) explodes
void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
	// TODO: spawn small enemies at the location of the input enemy e
	// when we create the smaller enemy, we have to read the values of the original enemy
	// - spawn a number of small enemies equal to the vertices of the original enemy
	// - set each small enemy to the same color as the original, half the size
	// - small enemies are worth double points of the original enemy
}

// spawns a bullet from a given entity to a target location 
void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2f& target) {

	// ToDo : implement the spawning of a bullet which travels toward target 
	// - buleet speed is given as a scalar speed 
	// - you must set the velocity by using formula in notes 


}

void Game::sMovement()
{
	// TODO: implement all entity movement in this function 
	// you should read the m_player->cInput component to determine if the player 

	// Sample movement speed update 
	// need to have a reference to actually modify it 
	auto& transform = player()->get<CTransform>();

	// transform.pos += transform.velocity; after implementation function in vec2 class  
	transform.pos.x += transform.velocity.x;
	transform.pos.y += transform.velocity.y;
}


void Game::sLifeSpan()
{
	// Todo: implement all lifespan functionality 
	//
	/// for all entities
	// if entity has no lifespan component, skip it 
	// if entity has > 0 remaining lifespan, subtract 1 
	// if it has lifespan and is alive 
	// sacle its alpha channel properly 
	// if it has lifespan and its time is up 
	// destroy the entity 
}

void Game::sCollision()
{
	// Todo: implement all proper colllisions between entities 
	// be sure to use the collision radius, Not the spae radius 
}

void Game::sEnemySpawner()
{
	// Todo: code which implements enemy spawning should go here 
}

void Game::sGUI() {
	ImGuiLLBegin("Geometry Wars");
	ImGui::Text("stuff goes here");
	ImGui::End();
}

void Game::sRender()
{
	// TODO: change the code below to draw ALL of the entities
	// sample drawing of the player Entity that we have created
	m_window.clear();

	// set the position of the shape based on the entity's transform->pos
	player()->get<CShape>().circle.setPosition(player()->get<CTransform>().pos);

	// set the rotation of the shape based on the entity's transform->angle
	player()->get<CTransform>().angle += 1.0f;
	player()->get<CShape>().circle.setRotation(player()->get<CTransform>().angle);

	// draw the entity's sf::CircleShape
	m_window.draw(player()->get<CShape>().circle);

	// draw the ui last 
	ImGui::SFML::Render(m_window);
	m_window.display();
}

// detec the button pressed 
void Game::sUserInput()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::Closed:
		{
			m_running = false;
			break;
		}
		case sf::Event::KeyPressed:
		{
			if (event.key.code == sf::Keyboard::Escape)
			{
				m_running = false;
			}
			else if (event.key.code == sf::Keyboard::P)
			{
				m_paused ? setPaused(false) : setPaused(true);
			}
			//Player input
			else if (event.key.code == sf::Keyboard::A) { m_player->cInput->left = true; }
			else if (event.key.code == sf::Keyboard::D) { m_player->cInput->right = true; }
			else if (event.key.code == sf::Keyboard::W) { m_player->cInput->up = true; }
			else if (event.key.code == sf::Keyboard::S) { m_player->cInput->down = true; }

			break;
		}
		case sf::Event::KeyReleased:
		{
			if (event.key.code == sf::Keyboard::A) { m_player->cInput->left = false; }
			else if (event.key.code == sf::Keyboard::D) { m_player->cInput->right = false; }
			else if (event.key.code == sf::Keyboard::W) { m_player->cInput->up = false; }
			else if (event.key.code == sf::Keyboard::S) { m_player->cInput->down = false; }
			break;
		}
		case sf::Event::MouseButtonPressed:
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				if (m_player->cInput->leftMouse == false)
				{
					m_player->cInput->leftMouse = true;
				}
			}
			if (event.mouseButton.button == sf::Mouse::Right)
			{
				if (m_player->cInput->rightMouse == false)
				{
					m_player->cInput->rightMouse = true;
				}
			}
			break;
		}
		case sf::Event::MouseButtonReleased:
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				m_player->cInput->leftMouse = false;
			}
			break;
		}

		default:
			break;
		}
	}
}
