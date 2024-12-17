#include <iostream>
#pragma once
#include <SFML/Graphics.hpp>
#include <math.h>
#include "vec2.hpp"
#include "Common.h"
#include "Game.h"
int main() {


	// Test Vec2 class 
	//Vec2<int> v1(10, 20);
	//Vec2<int> v2(20, 30);

	//Vec2<int> v3 = v1 + v2; 
	//std::cout << v3.x << v3.y;
	//v1 += v2;
	//std::cout << v1.x << v1.y;
	//float distance = v1.dist(v2);
	//std::cout << distance; 


	// Run the game 
	Game g("config.txt");
	g.run();

	return 0;
}