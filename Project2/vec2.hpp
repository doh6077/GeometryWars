#pragma once
#include <SFML/Graphics.hpp>
#include <math.h>
#include <cmath>
// By using the template class, the datatype wii be flexible 
// Vec2<int> v; 
// Vec2<float> v; 


template <typename T>
class Vec2
{

public:
	T x = 0;
	T y = 0;

	Vec2() = default;

	Vec2(T xin, T yin)
		: x(xin), y(yin)
	{
	}

	// constructor to convert from sf::Vector2
	Vec2(const sf::Vector2<T>& vec)
		:x(vec.x), y(vec.y)
	{
	}

	// allow automatic conversion from Vec2 to sf::Vector2
	// this lets us pass Vec2 into sfml functions 
	// if we used the customed vec2, it would have the compiler error 
	operator sf::Vector2<T>()
	{
		return sf::Vector2<T>(x, y);
	}

	// overloading the opeartors 
	Vec2 operator + (const Vec2& rhs) const
	{
	
		return Vec2(x+ rhs.x, y + rhs.y);
	}

	Vec2 operator - (const Vec2& rhs) const
	{
		return Vec2(x - rhs.x, y - rhs.y);
	}

	Vec2 operator * (const Vec2& rhs) const
	{
		return Vec2(x * rhs.x, y * rhs.y);
	}

	void operator += (const Vec2& rhs)
	{
		x += rhs.x;  // Add rhs.x to the current x component
		y += rhs.y;  // Add rhs.y to the current y component
	}
	void operator -= (const Vec2& rhs)
	{
		x -= rhs.x;  
		y -= rhs.y;  
	}

	void operator *= (const Vec2& rhs)
	{
		x *= rhs.x;
		y *= rhs.y;
	}

	void operator /= (const Vec2& rhs)
	{
		x /= rhs.x;
		y /= rhs.y;
	}

	float dist(const Vec2& rhs) const
	{
		float distance = sqrt(pow((x - rhs.x), 2) + pow((y - rhs.y), 2));
		return distance; 
	}
};

using Vec2f = Vec2<float>;