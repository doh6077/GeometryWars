#include <iostream>
#pragma once
#include <SFML/Graphics.hpp>
#include <math.h>
int main() {

	sf::Vector2f v1(16.5f, 24.f);
	sf::Vector2f v2(16.5f, 24.f);
	sf::Vector2f v3;
	v3 = v1 + v2;
	std::cout << v3.x;
	return 0;
}