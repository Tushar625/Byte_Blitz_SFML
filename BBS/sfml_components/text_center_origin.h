#pragma once

#include <SFML/Graphics.hpp>

#include <cmath>


namespace bb
{


// set the origin of the sfml text at its center

// visit: https://learnsfml.com/basics/graphics/how-to-center-text/

void setCenterOrigin(sf::Text& text)
{
	auto center = text.getGlobalBounds().getSize() / 2.f;
	auto localBounds = center + text.getLocalBounds().getPosition();
	auto rounded = sf::Vector2f{ std::round(localBounds.x), std::round(localBounds.y) };
	text.setOrigin(rounded);
}


}