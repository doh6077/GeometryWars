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

	//fin >> m_playerConfig.SR >> m_playerConfig.CR;

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
		//sEnemySpawner();
		//sCollision();
		//sGUI();
		sUserInput();
		sMovement();
		sRender();

		if (m_paused) {
			// stop the movement 
			
		}
		// increment the current frame 
		m_currentFrame++; 
	}
	ImGui::SFML::Shutdown();  // Cleanup ImGui

}

void Game::spawnPlayer()
{
	// TODO: Finish adding all properties of the player with the correct values from the config
	Vec2f startPosition(500.0f, 360.0f);
	Vec2f startVelocity(3.0f, 0.0f);
	sf::Color fillColor(0, 0, 0);      // Fill color (R, G, B)
	sf::Color outlineColor(255, 0, 0);   // Outline color (R, G, B)
	float outlineThickness = 3.0f;       // Outline thickness
	int shapeSides = 8;                  // Number of sides for the shape
	float shapeRadius = 32.0f;           // Radius of the shape
	float angle = 0.0f;                  // Initial angle
	// We create every entity by calling EntityManager.addEntity(tag)
	// This returns a std::shared_ptr<Entity>, so we use 'auto' to save typing
	auto entity = m_entities.addEntity("player");
	std::cout << "Player spawned at: " << startPosition.x << ", " << startPosition.y << "\n";
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

	// Starting position of the bullet
	Vec2f startPosition = entity->get<CTransform>().pos;

	// Calculate the direction vector from startPosition to target
	Vec2f direction = target - startPosition;

	// Bullet speed
	Vec2f bulletSpeed = { 2.0f, 2.0f };
	Vec2f velocity = direction * bulletSpeed;

	// Bullet appearance properties
	sf::Color fillColor(8, 255, 0);
	sf::Color outlineColor(0, 255, 100);
	float outlineThickness = 2.0f;      // Reduced outline thickness
	int shapeSides = 20;                // Reduced for a smaller circle-like shape
	float shapeRadius = 5.0f;           // Smaller bullet size

	// Create the bullet entity
	auto bullet = m_entities.addEntity("bullet");

	// Add components to the bullet entity
	bullet->add<CTransform>(startPosition, velocity, 0.0f); // Set velocity
	bullet->add<CShape>(shapeRadius, shapeSides, fillColor, outlineColor, outlineThickness);
	bullet->add<CLifespan>(9); // Bullet lifespan
}

void Game::sMovement()
{
	// Iterate through all entities in the game
	for (const auto& entity : m_entities.getEntities()) {
		// Check if the entity has a CTransform component
		if (entity->has<CTransform>()) {
			auto& transform = entity->get<CTransform>();

			// Update position based on velocity
			transform.pos += transform.velocity;
		}

		// Handle player-specific movement using CInput
		if (entity->tag() == "player" && entity->has<CInput>()) {
			auto& input = entity->get<CInput>();
			auto& transform = entity->get<CTransform>();

			// Reset velocity to zero before applying new input
			transform.velocity = Vec2f(0.0f, 0.0f);

			// Apply input to velocity
			if (input.up) {
				transform.velocity.y = -3.0f; // Move up
			}
			if (input.down) {
				transform.velocity.y = 3.0f;  // Move down
			}
			if (input.left) {
				transform.velocity.x = -3.0f; // Move left
			}
			if (input.right) {
				transform.velocity.x = 3.0f;  // Move right
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
	size_t size = m_entities.getTotalEntities();
	for (const auto& entity : m_entities.getEntities()) {
		if (entity->has<CLifespan>()) {
			if (entity->get<CLifespan>().lifespan > 0) {
				entity->get<CLifespan>().lifespan -= 1;

			}

		}
	}
}

void Game::sCollision()
{
	// Todo: implement all proper colllisions between entities 
	// be sure to use the collision radius, Not the spae radius 
	for (auto b : m_entities.getEntities("bullet")) {

		for (auto e : m_entities.getEntities("enemy")) {

			// detect the collision 
			if ((e->get<CCollision>().radius + b->get<CCollision>().radius) > e->get<CTransform>().pos.dist(b->get<CTransform>().pos)) {
				// detect the collision 
				std::cout << "collision has been detected\n";

			}


		}

		for (auto e : m_entities.getEntities("smallEnemy")) {
			if ((e->get<CCollision>().radius + b->get<CCollision>().radius) > e->get<CTransform>().pos.dist(b->get<CTransform>().pos)) {
				// detect the collision 
				std::cout << "collision has been detected\n";
			}
		}
	}

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

	for (auto entity: m_entities.getEntities()) {

		// set the position of the shape based on the entity's transform->pos
		//entity->get<CShape>().circle.setOrigin(entity->get<CTransform>().pos);
		entity->get<CShape>().circle.setPosition(entity->get<CTransform>().pos);
		// set the rotation of the shape based on the entity's transform->angle
		//entity->get<CTransform>().angle += 0.01f;
		//entity->get<CShape>().circle.setRotation(entity->get<CTransform>().angle);

		// draw the entity's sf::CircleShape
		m_window.draw(entity->get<CShape>().circle);


	}
	// ImGui rendering
	ImGui::SFML::Render(m_window);
	m_window.display();

}

// detect the button pressed 
void Game::sUserInput()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		// Pass the event to ImGui for processing
		ImGui::SFML::ProcessEvent(m_window, event);

		// Handle window close event
		if (event.type == sf::Event::Closed) {
			m_running = false;
		}
		// Handle key press events
		if (event.type == sf::Event::KeyPressed) {
			switch (event.key.code) {
			case sf::Keyboard::W:
				std::cout << "W Key Pressed\n";
				player()->get<CInput>().up = true;
				break;
			case sf::Keyboard::S:
				std::cout << "S Key Pressed\n";
				player()->get<CInput>().down = true;
				break;
			case sf::Keyboard::A:
				std::cout << "A Key Pressed\n";
				player()->get<CInput>().left = true;
				break;
			case sf::Keyboard::D:
				std::cout << "D Key Pressed\n";
				player()->get<CInput>().right = true;
				break;
			default:
				break;
			}
		}
		// Handle key release events
		if (event.type == sf::Event::KeyReleased) {
			switch (event.key.code) {
			case sf::Keyboard::W:
				std::cout << "W Key Released\n";
				player()->get<CInput>().up = false;
				break;
			case sf::Keyboard::S:
				std::cout << "S Key Released\n";
				player()->get<CInput>().down = false;
				break;
			case sf::Keyboard::A:
				std::cout << "A Key Released\n";
				player()->get<CInput>().left = false;
				break;
			case sf::Keyboard::D:
				std::cout << "D Key Released\n";
				player()->get<CInput>().right = false;
				break;
			default:
				break;
			}
		}



		// Handle mouse button press events
		if (event.type == sf::Event::MouseButtonPressed) {
			if (ImGui::GetIO().WantCaptureMouse) {
				continue;
			}

			if (event.mouseButton.button == sf::Mouse::Left) {
				std::cout << "Left Mouse Button Clicked at ("
					<< event.mouseButton.x << ", "
					<< event.mouseButton.y << ")\n";

				Vec2f target(static_cast<float>(event.mouseButton.x),
					static_cast<float>(event.mouseButton.y));
				spawnBullet(player(), target);
			}
		}
	}
}
