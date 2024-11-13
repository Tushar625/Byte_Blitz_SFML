
#pragma once

#include<SFML/Graphics.hpp>

namespace bb
{

	unsigned int set_antialiashing();

	// this object, WINDOW, should be used to access or modify the SFML window

	sf::RenderWindow WINDOW(
		sf::VideoMode(800, 600),
		"Default Window 800ps-600px, Enjoy :)",
		sf::Style::Close,
		([]() -> sf::ContextSettings
			{
				sf::ContextSettings setting;
				setting.antialiasingLevel = set_antialiashing();
				return setting;
			}
		)()	/*creating this lambda expression to return sf::ContextSettings object and calling it*/
	);

}

#ifndef SET_ANTIALIASHING

	// redefine this function to change antialiashing setting

	inline unsigned int bb::set_antialiashing()
	{
		return 8;	// return the antialiashing value
	}

#endif