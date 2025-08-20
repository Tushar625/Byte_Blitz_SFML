#pragma once


#include<SFML/Graphics.hpp>
#include"../../../utility/pos_fun.h"
#include"../../../system/window.h"
#include<vector>
#include<string>
#include<cctype>


namespace bb
{
	class ColorText;
}


/*
	ColorText class is used to create a text object that displays each character
	in a different color. It is useful for creating colorful text effects.

	It allows you to set the set different colors to each of the characters in the text.

	You can shift the character colors left or right at certain interval to create a dynamic effect.

	Example usage:
	
	ColorText colorText;

	colorText.set(someText, "Hello World", 5, 100, 100, bb::TOP_LEFT);	// check set() for more details

	// set colors for each character

	colorText.setColor({ sf::Color::Red, sf::Color::Green, sf::Color::Blue });

	// update the colors every 0.075 seconds

	colorText.updateRS(dt);	// call this in update() of your game loop

	// render all the text objects

	colorText.render();	// call this in render() of your game loop
*/

class bb::ColorText
{


	std::vector<sf::Text> m_textList;	// each character is a separate text object

	double m_time;	// time since last color change
	
	double m_interval;	// the interval between color changes



public:



	ColorText() : m_time(0), m_interval(.075)/* default interval */
	{ }



	/*
		sets the text objects with each character of the given string.

		@param text: the sf::Text object, whose properties will be copied to the text objects for each character
		@param str: the string to be displayed
		@param gap: the gap between each character
		@param x, y: where the text will be positioned
		@param pos: the position of the text box, that x, y represents. 
			For example, if pos is TOP_LEFT, then x, y will be the top left corner of the text box.
			If pos is CENTER, then x, y will be the center of the text box.
			See pos_fun.h for more details on COORD_POSITION enum.
	*/

	void set(sf::Text text, const std::string& str, float gap, const sf::Vector2f& coord, bb::COORD_POSITION pos = bb::TOP_LEFT)
	{
		float x, y, width, height;

		m_textList.clear();	// clear previous text objects

		/*
			assuming top left point of the text box is at (0, 0), we position the characters
			i.e., the text objects.

			height of the text box is the height of the tallest character
		*/

		x = height = 0;

		for (const auto& ch : str)
		{
			text.setString(std::string(1, ch));

			if (!std::isspace(ch))
			{
				text.setOrigin(text.getLocalBounds().getPosition());

				text.setPosition(x, 0);

				m_textList.push_back(text);
			}

			// update position x

			x += text.getLocalBounds().width + gap;

			// update height

			if (height < text.getLocalBounds().height)
			{
				height = text.getLocalBounds().height;
			}
		}

		width = x - gap;	// no gap after the last character

		// Now we have height and width, lets get the real top left position of the text box.

		bb::to_top_left(x, y, coord.x, coord.y, height, width, pos);

		// moving the texts to their actual position, after we figure out the top left coordinates.

		for (auto& text : m_textList)
		{
			text.move(x, y);
		}
	}



	/*
		setting the initial colors of the characters or the text objects.

		@param colors: a vector of colors to be set for each character.

			If the size of the vector is less than the number of characters, the colors will be repeated.

			If the size is greater than the number of characters, only the first n colors will be used, 
			where n is the number of characters.
	*/

	void setColor(const std::vector<sf::Color>& colors)
	{
		int i = 0;

		for (auto& text : m_textList)
		{
			text.setFillColor(colors[i++ % colors.size()]);	// repeat colors if necessary
		}
	}



	/*
		sets the interval between color changes.
	*/

	void setInterval(double interval)
	{
		m_interval = interval;
	}



	/*
		left shift the colors of the text objects, and the first color will be moved to the end.
	*/

	void colorLS()
	{
		auto temp = m_textList[0].getFillColor();

		for (int i = 1; i < m_textList.size(); i++)
		{
			m_textList[i - 1].setFillColor(m_textList[i].getFillColor());
		}

		m_textList[m_textList.size() - 1].setFillColor(temp);
	}



	/*
		right shift the colors of the text objects, and the last color will be moved to the front.
	*/

	void colorRS()
	{
		auto temp = m_textList[m_textList.size() - 1].getFillColor();

		for (int i = m_textList.size() - 1; i > 0; i--)
		{
			m_textList[i].setFillColor(m_textList[i - 1].getFillColor());
		}

		m_textList[0].setFillColor(temp);
	}



	/*
		update the time and right shift color of the text objects if the time exceeds the interval.
	*/

	void updateRS(double dt)
	{
		m_time += dt;

		if (m_time > .075)
		{
			m_time = 0;

			colorRS();
		}
	}


	/*
		update the time and left shift color of the text objects if the time exceeds the interval.
	*/

	void updateLS(double dt)
	{
		m_time += dt;

		if (m_time > .075)
		{
			m_time = 0;

			colorLS();
		}
	}



	/*
		get the text object at the given index.
	*/

	sf::Text getText(int index) const noexcept
	{
		return m_textList[index];
	}



	/*
		get the number of text objects.
	*/
	
	int getTextCount() const noexcept
	{
		return static_cast<int>(m_textList.size());
	}



	/*
		render the text objects to the window.
	*/

	void render()
	{
		for (const auto& text : m_textList)
		{
			bb::WINDOW.draw(text);
		}
	}
};