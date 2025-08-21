#pragma once

#include<SFML/Graphics.hpp>
#include"../../system/window.h"



namespace bb
{

/*
	this function draws several dark colored texts at different offsets
	around the original text, creating a shadow effect.

	@param text: the original text to be shadowed
	@param offsets: a vector of offsets from original text where the shadow texts will be drawn
	@param color: the color of the shadow texts, default is black

	Usage:
	
	textShadow(myText, { {1.5, 1.5}, {1.5, 1}, {0, 1.5} });
		
	bb::WINDOW.draw(myText);	// actual text is drawn last to ensure it appears on top of the shadows
*/

void textShadow(sf::Text text, const std::vector<sf::Vector2f>& offsets, const sf::Color& color = sf::Color(0, 0, 0))
{
	text.setFillColor(color);

	auto pos = text.getPosition();

	for (const auto& offset : offsets)
	{
		text.setPosition(pos + offset);

		bb::WINDOW.draw(text);
	}
}

}