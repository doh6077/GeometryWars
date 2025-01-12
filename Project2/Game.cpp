#include "Game.h"
#include "Common.h"
#include <iostream>
#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"
#include "imgui-SFML.h"

// generate random number for the enemy position 
#include <random>

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
	m_scoreText.setPosition(0, 0);
	m_scoreText.setString(std::to_string(m_score));
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
		if (!m_paused)
		{
			sLifeSpan();
			sEnemySpawner();
			sMovement();
			sCollision();
		}
		sUserInput();
		sRender();
		// increment the current frame 
		m_currentFrame++; 
	}
	ImGui::SFML::Shutdown();  // Cleanup ImGui

}

void Game::spawnPlayer()
{
	// TODO: Finish adding all properties of the player with the correct values from the config
	Vec2f middleOfWindow{ m_window.getSize().x / 2.0f, m_window.getSize().y / 2.0f };
	Vec2f startVelocity(0.0f, 0.0f);
	sf::Color fillColor(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB);      // Fill color (R, G, B)
	sf::Color outlineColor(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB);   // Outline color (R, G, B)
	float outlineThickness = m_playerConfig.OT;       // Outline thickness
	int vertices = m_playerConfig.S;                  // Number of sides for the shape
	float shapeRadius = m_playerConfig.SR          // Radius of the shape
	float angle = 0.0f;                  // Initial angle
	// We create every entity by calling EntityManager.addEntity(tag)
	// This returns a std::shared_ptr<Entity>, so we use 'auto' to save typing
	auto entity = m_entities.addEntity("player");
	std::cout << "Player spawned at: " << middleOfWindow.x << ", " << middleOfWindow.y << "\n";
	// Add components to the entity
	entity->add<CTransform>(middleOfWindow, startVelocity, angle);
	entity->add<CShape>(shapeRadius, vertices, fillColor, outlineColor, outlineThickness);

	// Add an input component to the player so that we can use inputs
	entity->add<CInput>();

	
}


// spawn an enemy at a random position 
void Game::spawnEnemy()
{
	// Todo: make sure the enemy is spawned properly with the m_enemyConfig variables 
	// the enemy must be spawned completely within the bounds of the window 
	std::random_device rd;
	std::mt19937 mt(rd());
	// Define range for random position (example: x between 100-500, y between 100-400)
	std::uniform_int_distribution<int> distX(100, 1280);  // Range for x-coordinate
	std::uniform_int_distribution<int> distY(100, 720);  // Range for y-coordinate

	// Generate random start position
	Vec2f startPosition(distX(mt), distY(mt));
	//Vec2f startPosition(200.0f, 360.0f);   // Position (x, y)
	Vec2f startVelocity(0.05f, 0.0f);   // Velocity (x, y)
	sf::Color fillColor(255, 255, 255); // Fill color (white)
	sf::Color outlineColor(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OT);    // Outline color (dark purple/blue)
	float shapeRadius = m_enemyConfig.SR;          // Enemy size (use a default if not specified)

	// Create the enemy entity
	auto enemy = m_entities.addEntity("enemy");
	


	// set Vertices 
	auto minVertices = m_enemyConfig.VMIN; 
	auto minVertices = m_enemyConfig.VMAX;
	// Set up the random number generator
	std::random_device rd;  // Obtain a seed from the system
	std::mt19937 gen(rd()); // Initialize the generator
	std::uniform_int_distribution<> dis(minVertices, maxVertices); // Define the range

	// Generate the random value
	auto vertices = dis(gen);

	// set Velocity 
	auto minVelocity = m_enemyConfig.VMIN;
	auto minVelocity = m_enemyConfig.VMAX;
	std::uniform_int_distribution<> dis(minVelocity, minVelocity); // Define the range
	auto velocity = dis(gen);
	Vec2f startVelocity{ velocity , velocity };
	// Set Color randomly 
	uint8_t minColor = 0;
	uint8_t maxColor = 255;
	uint8_t r = minColor + (std::rand() % (1 + maxColor - minColor));
	uint8_t g = minColor + (std::rand() % (1 + maxColor - minColor));
	uint8_t b = minColor + (std::rand() % (1 + maxColor - minColor));

	auto fillColor = sf::Color(r, g, b);



	// Add components to the enemy entity
	enemy->add<CTransform>(startPosition, startVelocity, 0.0f); // Default angle is 0.0f
	enemy->add<CShape>(shapeRadius, shapeSides, fillColor, outlineColor, outlineThickness);
	enemy->add<CCollision>(shapeRadius);

	// add score component to give player the score when collision happens 
	int score = 5; 
	enemy->add<CScore>(score);
	enemy->add<CShape>(shapeRadius, vertices, fillColor, outlineColor, m_enemyConfig.OT);
	enemy->add<CCollision>(m_enemyConfig.CR);
	enemy->add<CLifespan>(m_enemyConfig.L);
	enemy->add<CScore>(vertices * 100);


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

	//Spawn a number of small enemies equal to the number of vertices of the original
	size_t vertices = e->get<CShape>().circle.getPointCount() ;

	// debugging 
	std::cout << "count POint" <<vertices;

	// 25.01.10 update this part to reflect the parent enemy attributes 
	Vec2f startPosition(e->get<CTransform>().pos);
	//Vec2f startPosition(200.0f, 360.0f);   // Position (x, y)
	Vec2f startVelocity(e->get<CTransform>().velocity);   // Velocity (x, y)
	sf::Color fillColor(255, 255, 255); // Fill color (white)
	sf::Color outlineColor(3, 3, 9);    // Outline color (dark purple/blue)
	float outlineThickness = 3.0f;     // Outline thickness
	int shapeSides = 3;                // Number of sides (e.g., a circle-like shape)
	float shapeRadius = e->get<CShape>().circle.getRadius() * 0.5;         // Enemy size (use a default if not specified)

	for (size_t i = 0; i < vertices; ++i)
	{
		// Create the small enemy entity
		auto enemy = m_entities.addEntity("smallEnemy");

		// Add components to the enemy entity
		enemy->add<CTransform>(startPosition, startVelocity, 0.0f); // Default angle is 0.0f
		enemy->add<CShape>(shapeRadius, shapeSides, fillColor, outlineColor, outlineThickness);
		enemy->add<CCollision>(shapeRadius);

		// add score component to give player the score when collision happens 
		int score = 5 * 2;
		enemy->add<CScore>(score);
	}

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
	bullet->add<CLifespan>(20); // Bullet lifespan
	bullet->add<CCollision>(shapeRadius);
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

	// if there's a collision between enemy and player 
	// Destroy the player, destroy the enemy, respawn the player 
	for (auto e : m_entities.getEntities("enemy")) {

		for (auto p : m_entities.getEntities("player")) {
			if (!e->isActive()) continue; // Skip inactive enemies
			float distance = e->get<CTransform>().pos.dist(p->get<CTransform>().pos);

			float collisionRadius = e->get<CCollision>().radius + p->get<CCollision>().radius;

			// detect a collision 
			if (distance < collisionRadius) {
				// when collision happens the score becomes reset 
				m_score = 0; 
				m_scoreText.setString(std::to_string(m_score));
				std::cout << "Score has been updated\n";

				spawnSmallEnemies(e);
				std::cout << "collision has been detected between enemy and player\n";
				p->destroy();
				e->destroy();
				break;
				spawnPlayer();

			}
		}
	}
	// if there's a collision between smallEnemy and player 
	// Destroy the player, destroy the enemy, respawn the player 
	for (auto e : m_entities.getEntities("smallEnemy")) {

		for (auto p : m_entities.getEntities("player")) {
			if (!e->isActive()) continue; // Skip inactive enemies
			float distance = e->get<CTransform>().pos.dist(p->get<CTransform>().pos);

			float collisionRadius = e->get<CCollision>().radius + p->get<CCollision>().radius;

			// detect a collision 
			if (distance < collisionRadius) {
				// when collision happens the score becomes reset 
				m_score = 0;
				m_scoreText.setString(std::to_string(m_score));
				std::cout << "Score has been updated\n";

				spawnSmallEnemies(e);
				std::cout << "collision has been detected between enemy and player\n";
				p->destroy();
				e->destroy();
				break;
				spawnPlayer();

			}
		}
	}
	// if there's collision between bullet and enemy 
	for (auto b : m_entities.getEntities("bullet")) {

		for (auto e : m_entities.getEntities("enemy")) {
			if (!e->isActive()) continue; // Skip inactive enemies
			float distance = e->get<CTransform>().pos.dist(b->get<CTransform>().pos);
			
			float collisionRadius = e->get<CCollision>().radius + b->get<CCollision>().radius;
			// detect a collision 
			if (distance < collisionRadius) {
				m_score += e->get<CScore>().score;
				m_scoreText.setString(std::to_string(m_score));
				std::cout << "Score has been updated\n";

				spawnSmallEnemies(e);
				std::cout << "collision has been detected\n";
				b->destroy();
				e->destroy();
				break; 

			}
		}
		// detect the collision between smallEnemy and bullet 
		for (auto e : m_entities.getEntities("smallEnemy")) {
			if ((e->get<CCollision>().radius + b->get<CCollision>().radius) > e->get<CTransform>().pos.dist(b->get<CTransform>().pos)) {
				// detect the collision 
				std::cout << "collision has been detected\n";

				// update the score 
				m_score += e->get<CScore>().score;
				m_scoreText.setString(std::to_string(m_score));
				std::cout << "Score has been updated\n";
				b->destroy();
				e->destroy();
			}
		}
	}

}
void Game::sEnemySpawner()
{
	// Todo: code which implements enemy spawning should go here 

	// for now, only generates one enemy
	// will be edited
	if ((m_currentFrame - m_lastEnemySpawnTime) >= 100)
	{
		spawnEnemy();
	}
	
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
	m_window.draw(m_scoreText);
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
