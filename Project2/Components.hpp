#pragma once 

#include "vec2.hpp"
#include <SFML/Graphics.hpp>

// Basic class component 
class Component
{
public:
	// whether or not the entity contains that 
	bool exists = false;
};

class CTransform : public Component
{
public:
	Vec2f pos = { 0.0, 0.0 };
	Vec2f velocity = { 0.0, 0.0 };
	float angle = 0; 

	CTransform() = default; 
	CTransform(const Vec2f &p, const Vec2f & v, float a)
		: pos(p), velocity(v), angle(a){ }
};

class CShape : public Component
{
public:
    sf::CircleShape circle;

    // Default constructor
    CShape() = default;

    // Parameterized constructor
    CShape(float radius, size_t points, const sf::Color& fill, const sf::Color& outline, float thickness)
        : circle(radius) // Correctly initialize circle with radius
    {
        circle.setPointCount(points);            // Set the number of points (makes it polygon-like)
        circle.setFillColor(fill);               // Set the fill color
        circle.setOutlineColor(outline);         // Set the outline color
        circle.setOutlineThickness(thickness);   // Set the outline thickness
        circle.setOrigin(radius, radius);        // Set the origin to the center
    }
};

class CCollision : public Component
{

public:
    float radius = 0;

    CCollision() = default;
    CCollision(float r)
        : radius(r) {
    }
};

class CScore : public Component
{
public:
    int score = 0;

    CScore() = default;
    CScore(int s)
        :score(s) {

    }
};


class CLifespan : public Component
{

public:
    int lifespan = 0;
    int remaining = 0;
    CLifespan() = default;
    CLifespan(int totalLifespan)
        :lifespan(totalLifespan), remaining(totalLifespan) {
    }
};


class CInput : public Component
{
public:
    bool up = false;
    bool left = false;
    bool right = false;
    bool down = false;
    bool shoot = false;

    CInput() = default;
};