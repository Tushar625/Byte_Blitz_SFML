
#pragma once

#include<SFML/Graphics.hpp>

sf::ContextSettings setting;

unsigned int set_antialiashing();

sf::RenderWindow WINDOW(sf::VideoMode(800, 600), "Default Window 800ps-600px, Enjoy :)", sf::Style::Close, (setting.antialiasingLevel = set_antialiashing(), setting));

#ifndef SET_ANTIALIASHING

	// redefine this function to change antialiashing setting

	inline unsigned int set_antialiashing()
	{
		return 8;	// return the antialiashing value
	}

#endif