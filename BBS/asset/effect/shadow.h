#pragma once

#include<SFML/Graphics.hpp>
#include"../../system/window.h"
#include<concepts>
#include<vector>



namespace bb
{



/*
	this function draws several dark colored shapes at different offsets
	around the original shape, creating a shadow effect.

	it has two overloads:

	overload 1 (variadic):

	@param text: the original text to be shadowed
	@param color: the color of the shadow texts
	@param offsets: variable no. of offsets from original text where the shadow texts will be drawn

	Usage:

	shadow(myRectangle, sf::Color(0, 0, 0), sf::Vector2f{1.5, 1.5}, sf::Vector2f{1.5, 1}, sf::Vector2f{0, 1.5});

	bb::WINDOW.draw(myRectangle);	// actual shape is drawn last to ensure it appears on top of the shadows

	overload 2:

	@param text: the original text to be shadowed
	@param offsets: a vector of offsets from original text where the shadow texts will be drawn
	@param color: the color of the shadow texts, default is black

	Usage:
	
	shadow(myText, { {1.5, 1.5}, {1.5, 1}, {0, 1.5} });
		
	bb::WINDOW.draw(myText);	// actual shape is drawn last to ensure it appears on top of the shadows

	note:

	The variadic version is slightly faster as it avoids the overhead of a vector
*/



template<typename Ts, typename... Tv> requires (std::same_as<Tv, sf::Vector2f> && ...)

void shadow(Ts shape, const sf::Color& color, const Tv&... offset) noexcept
{
	if constexpr (std::same_as<Ts, sf::Sprite>)
	{
		shape.setColor(color);
	}
	else
	{
		shape.setFillColor(color);
	}

	auto pos = shape.getPosition();

	((shape.setPosition(pos + offset), bb::WINDOW.draw(shape)), ...);
}



template<typename Ts>

void shadow(Ts shape, const std::vector<sf::Vector2f>& offsets, const sf::Color& color = sf::Color(0, 0, 0))
{
	if constexpr (std::same_as<Ts, sf::Sprite>)
	{
		shape.setColor(color);
	}
	else
	{
		shape.setFillColor(color);
	}

	auto pos = shape.getPosition();

	for (const auto& offset : offsets)
	{
		shape.setPosition(pos + offset);

		bb::WINDOW.draw(shape);
	}
}



}