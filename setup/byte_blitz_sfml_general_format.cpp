
/*
	a general format only, you should remove or change any code as you need
*/

#define SET_ANTIALIASHING	// this macro allows you to set antialiashing

#include"BBS\game.h"	// mention path to game.h

#include<iostream>	// only for debugging

#include<string>

#define HEIGHT 600

#define WIDTH 800

// the value returned by this function will be set as the value for antialiashing

unsigned int bb::set_antialiashing()
{
	return 8;
}

sf::Font font;

sf::Text text;

// ~~~~ [write your statements (extra functions, classes, variables) here] ~~~~

inline bool bb::Game::Create()
{
	bb::WINDOW.setSize(sf::Vector2u(WIDTH, HEIGHT));

	srand(time(0));

	bb::MY_GAME.set_fps(30);

	// WINDOW.setTitle("Name of your window ...");

	if (!font.loadFromFile("your font.ttf"))
	{
		std::cout << "can't load font\n";
	}

	text.setFont(font);

	text.setCharacterSize(20);

	text.setFillColor(sf::Color::White);

	// ~~~~ [write your statements here] ~~~~

	return SUCCESS;
}

inline bool bb::Game::Update(double dt)
{
	// exit

	if (bb::INPUT.isClosed() || bb::INPUT.isPressed(sf::Keyboard::Scan::Escape))
		return STOP_GAME_LOOP;

	// ~~~~ [write your statements here] ~~~~

	return !STOP_GAME_LOOP;
}

inline void bb::Game::Render()
{
	text.setString("Fps: " + std::to_string(bb::MY_GAME.get_fps()));

	bb::WINDOW.draw(text);

	// ~~~~ [write your statements here] ~~~~
}