# GeometryWars
This is a project I worked on during ubisoft mentorship.
A C++ game clone inspired by Geometry Wars, built with the SFML graphics library.

## Overview

GeometryWars is an action shooter where you control a ship that battles waves of geometric enemies. The game features a clean Entity-Component System architecture and config-driven gameplay parameters.

## Core Architecture

### Entity-Component System (ECS)
- **Entities**: Game objects (player, enemies, bullets) composed of modular components
- **Components**: Reusable data modules (CTransform, CShape, CCollision, CLifespan, CInput, CScore)
- **EntityManager**: Manages all game entities with tag-based organization for efficient querying

### System Functions
Game logic is separated into dedicated systems:
- **sMovement**: Handles entity position updates
- **sCollision**: Detects and resolves collisions between entities
- **sRender**: Draws all entities to the screen
- **sUserInput**: Processes keyboard and mouse input
- **sEnemySpawner**: Spawns enemies at intervals
- **sLifeSpan**: Manages entity lifetime and fading effects

## Key Features

- Player-controlled ship that shoots bullets toward mouse cursor
- Enemy spawning system with randomized polygon shapes (3-8 vertices)
- Enemies break into smaller fragments when destroyed
- Config file-driven design (window size, entity properties, colors, speeds)
- Score tracking system
- ImGui integration for debug UI

## Technical Highlights

- **Custom Vec2 Template Class**: 2D math operations (addition, distance, normalization)
- **Component-Based Design**: Uses `std::tuple` for efficient component storage
- **SFML CircleShape with setPointCount()**: Renders polygons (triangles, squares, pentagons, etc.)
- **Separation of Concerns**: Each game mechanic isolated into its own system

## Configuration

The game reads from `Config1.txt` to set up:
- Window dimensions and framerate
- Player properties (size, speed, colors, vertices)
- Enemy properties (size range, speed range, spawn interval, vertex count)
- Bullet properties (size, speed, lifespan, fire rate)

## Building

This is a Visual Studio solution (`.sln`) using SFML. Ensure SFML is properly linked in your project settings.

## How to Play

- **WASD**: Move the player ship
- **Mouse**: Aim direction
- **Left Click**: Shoot bullets
- Destroy enemies to score points
- Avoid colliding with enemies
