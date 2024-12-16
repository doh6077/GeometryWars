#include "Game.h"
#include "Common.h"
#include <iostream>
#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"
#include "imgui-SFML.h"

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
	ImGui::GetIO().FontGlobalScale = 2.0f;

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
	Vec2f startPosition(200.0f, 200.0f);
	Vec2f startVelocity(3.0f, 0.0f);
	sf::Color fillColor(0, 255, 0);      // Fill color (R, G, B)
	sf::Color outlineColor(0, 255, 0);   // Outline color (R, G, B)
	float outlineThickness = 3.0f;       // Outline thickness
	int shapeSides = 8;                  // Number of sides for the shape
	float shapeRadius = 32.0f;           // Radius of the shape
	float angle = 0.0f;                  // Initial angle
	// We create every entity by calling EntityManager.addEntity(tag)
	// This returns a std::shared_ptr<Entity>, so we use 'auto' to save typing
	auto entity = m_entities.addEntity("player");

	// Add components to the entity
	entity->add<CTransform>(startPosition, startVelocity, angle);
	entity->add<CShape>(shapeRadius, shapeSides, fillColor, outlineColor, outlineThickness);

	// Add an input component to the player so that we can use inputs
	entity->add<CInput>();
}


// spawn an enemy at a random position 
void Game::spawnEnemy()
{
	// Todo: make sure the enemy is spawned properly with the m_enemyConfig variables 
	// the enemy must be spawned completely within the bounds of the window 
	Vec2f startPosition(35.0f, 35.0f);   // Position (x, y)
	Vec2f startVelocity(-1.0f, 1.0f);   // Velocity (x, y)
	sf::Color fillColor(255, 255, 255); // Fill color (white)
	sf::Color outlineColor(3, 3, 9);    // Outline color (dark purple/blue)
	float outlineThickness = 90.0f;     // Outline thickness
	int shapeSides = 50;                // Number of sides (e.g., a circle-like shape)
	float shapeRadius = 32.0f;          // Enemy size (use a default if not specified)

	// Create the enemy entity
	auto enemy = m_entities.addEntity("enemy");

	// Add components to the enemy entity
	enemy->add<CTransform>(startPosition, startVelocity, 0.0f); // Default angle is 0.0f
	enemy->add<CShape>(shapeRadius, shapeSides, fillColor, outlineColor, outlineThickness);
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
	Vec2f bulletSpeed = { 300.0f, 300.0f };
	// - buleet speed is given as a scalar speed 
	// - you must set the velocity by using formula in notes 
	Vec2f startPosition(entity->get<CTransform>().pos);
	// Calculate the direction vector from startPosition to target
	Vec2f direction = target - startPosition;
	Vec2f startVelocity(-1.0f, 1.0f);   // Velocity (x, y)
	float distance = startPosition.dist(target);
	// Scale the normalized direction vector by bulletSpeed to calculate velocity
	Vec2f velocity = direction * bulletSpeed;

	// Bullet appearance properties
	sf::Color fillColor(255, 255, 255); // White
	sf::Color outlineColor(3, 3, 9);    // Dark purple/blue
	float outlineThickness = 3.0f;      // Outline thickness
	int shapeSides = 50;                // Circle-like shape
	float shapeRadius = 5.0f;           // Bullet size

	// Create the bullet entity
	auto bullet = m_entities.addEntity("bullet");

	// Add components to the bullet entity
	bullet->add<CTransform>(startPosition, velocity, 0.0f); // Set velocity
	bullet->add<CShape>(shapeRadius, shapeSides, fillColor, outlineColor, outlineThickness);
}

void Game::sMovement()
{
	// Iterate through all entities in the game
	for (const auto& entity :  .getAllEntities()) {
		// Check if the entity has a CTransform component
		if (entity->hasComponent<CTransform>()) {
			auto& transform = entity->get<CTransform>();

			// Update position based on velocity
			transform.position.x += transform.velocity.x;
			transform.position.y += transform.velocity.y;
		}

		// Optional: Handle player-specific movement using CInput
		if (entity->tag() == "player" && entity->hasComponent<CInput>()) {
			auto& input = entity->get<CInput>();

			// Example: Update velocity based on input
			if (input.isKeyPressed(sf::Keyboard::W)) {
				entity->get<CTransform>().velocity.y = -1.0f; // Move up
			}
			if (input.isKeyPressed(sf::Keyboard::S)) {
				entity->get<CTransform>().velocity.y = 1.0f;  // Move down
			}
			if (input.isKeyPressed(sf::Keyboard::A)) {
				entity->get<CTransform>().velocity.x = -1.0f; // Move left
			}
			if (input.isKeyPressed(sf::Keyboard::D)) {
				entity->get<CTransform>().velocity.x = 1.0f;  // Move right
			}
		}
	}
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
	ImGui::Begin("Geometry Wars");
	ImGui::Text("stuff goes here");
	ImGui::End();
}

void Game::sRender()
{
	m_window.clear();
	// TODO: change the code below to draw ALL of the entities
	size_t size = m_entities.getTotalEntities();
	for (std::size_t i = 0; i < size; ++i) {
		auto entity = m_entities.getEntities()[i];
		// set the position of the shape based on the entity's transform->pos
		entity->get<CShape>().circle.setOrigin(entity->get<CTransform>().pos);
		// set the rotation of the shape based on the entity's transform->angle
		entity->get<CTransform>().angle += 1.0f;
		entity->get<CShape>().circle.setRotation(entity->get<CTransform>().angle);

		// draw the entity's sf::CircleShape
		m_window.draw(entity->get<CShape>().circle);

	}

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
