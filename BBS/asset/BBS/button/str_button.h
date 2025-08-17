#pragma once

#include<SFML/Graphics.hpp>
#include"../../../utility/pos_fun.h"
#include"../../../button/button.h"
#include"../../../system/window.h"
#include<vector>
#include<string>


namespace bb
{	
	class STR_BUTTON;
}



// creating the string buttons

/*
	STR_BUTTON is a button that displays a string and changes its color when hovered over.

	This button has only two states: ORDINARY_STATE and HOVERING_STATE.
	When the mouse pointer is over the button, it changes to HOVERING_STATE, otherwise it
	remains in ORDINARY_STATE.

	you can override the ORDINARY_STATE and HOVERING_STATE functions to change how the button looks
	when it is in those states.

	Example:

		auto button = STR_BUTTON(
				medium_text,
				"Start",
				sf::Color(48, 96, 130),
				sf::Color(99, 155, 255),
				sf::Vector2f(x, y),
				bb::TOP_CENTER
			)

		This creates a button with the text "Start", at (x, y) point on the screen, where (x, y)
		represents the top center point of the button, with ordinary color as sf::Color(48, 96, 130)
		and hover color as sf::Color(99, 155, 255).

		The sf::Text object "medium_text" should be preconfigured with the desired font, character
		size and other settings. The "button_text" object, inside STR_BUTTON, will copy the settings
		from "medium_text" and use them to display the button text.
*/

class bb::STR_BUTTON : public bb::BUTTON
{
	sf::Text button_text;	// the text displayed by the button

	const sf::Color ordinary_color, hover_color;	// colors for the button in different states

	/*
		button_color, allows the MENU selecter function to change ORDINARY_STATE button color
	*/

	sf::Color button_color;



	virtual void ORDINARY_STATE()
	{
		button_text.setPosition(sf::Vector2f(get_x(), get_y()));

		button_text.setFillColor(button_color);

		bb::WINDOW.draw(button_text);

		/*
			if the button_list indicator doesn't change the button_color
			afterwards, the button color will remain ordinary_color
		*/

		button_color = ordinary_color;
	}



	virtual void HOVERING_STATE() override
	{
		button_text.setPosition(sf::Vector2f(get_x(), get_y()));

		button_text.setFillColor(hover_color);

		bb::WINDOW.draw(button_text);
	}



public:

	

	STR_BUTTON(const sf::Text& text, const std::string& button_str, const sf::Color& _ordinary_color, const sf::Color& _hover_color, const sf::Vector2f& coord, bb::COORD_POSITION pos = bb::TOP_LEFT)
	:	ordinary_color(_ordinary_color),
		hover_color(_hover_color)
	{
		// preparing the button text

		button_text = text;

		button_text.setString(button_str);

		/*
			by default origin of a text stays at (0, 0), but top-left corner of local bounds
			is not (0, 0), this leaves some blank space above the text in the button, to fix
			this I use the top-left corner of local bounds as the origin
		*/

		button_text.setOrigin(button_text.getLocalBounds().getPosition());

		// setting size of the button

		/*
			actual height of text usually less than character size set for the text object
			so we use text height to set the button height instead of character size
		*/

		set_height(button_text.getLocalBounds().height);

		set_width(button_text.getLocalBounds().width);

		// setiing button position

		set_pos(coord.x, coord.y, pos);
	}



	sf::Text get_text() const noexcept
	{
		return button_text;
	}



	// pointer for menu as a static member function

	
	
	// >>> button <<<

	static void pointer(bb::MENU<STR_BUTTON>& menu)
	{
		STR_BUTTON& button = menu.get_mbutton();

		sf::Text text = button.button_text;

		text.setFillColor(button.hover_color);

		text.setString(">>>");

		text.setPosition(sf::Vector2f(button.get_x() - text.getLocalBounds().width - 20, button.get_y()));

		bb::WINDOW.draw(text);

		text.setString("<<<");

		text.setPosition(sf::Vector2f(button.get_x() + button.get_width() + 20, button.get_y()));

		bb::WINDOW.draw(text);

		// change the original state color to indicate that it's selected

		button.button_color = button.hover_color;
	}



	// ----------
	// | button |
	// ----------

	static void box(bb::MENU<STR_BUTTON>& menu)
	{
		STR_BUTTON& button = menu.get_mbutton();

		// the text seems to have 6 pixels of padding on the top so we add 6 to the height instead of 12

		sf::RectangleShape box(sf::Vector2f(button.get_width() + 12, button.get_height() + 6));

		box.setPosition(sf::Vector2f(button.get_x() - 6, button.get_y()));

		box.setOutlineThickness(1);

		box.setOutlineColor(button.hover_color);

		box.setFillColor(sf::Color::Transparent);

		bb::WINDOW.draw(box);

		// change the original state color to indicate that it's selected

		button.button_color = button.hover_color;
	}



	// button (with hover color)

	static void color(bb::MENU<STR_BUTTON>& menu)
	{
		STR_BUTTON& button = menu.get_mbutton();

		// change the original state color to indicate that it's selected

		button.button_color = button.hover_color;
	}



	/*
		creates a menu with center aligned list of buttons

		Example:-

		auto menu = bb::STR_BUTTON::make_menu(
				medium_text,
				{ "Continue", "Start", "Highest Score", "Quite" },
				sf::Color(48, 96, 130),
				sf::Color(99, 155, 255),
				10,
				sf::Vector2f(VIRTUAL_WIDTH / 2, VIRTUAL_HEIGHT / 2 + 10),
				bb::TOP_CENTER
			);
	*/

	static bb::MENU<STR_BUTTON> make_menu(sf::Text text, const std::vector<std::string>& str_list, const sf::Color& ordinary_color, const sf::Color& hover_color, float gap, const sf::Vector2f& coord, bb::COORD_POSITION pos = bb::TOP_LEFT)
	{
		/*
			getting height and width of the menu

			Height:

				each button has a height of text.getCharacterSize() and we add a "gap" after them,
				so the total height is "(height of text + gap) * number of buttons - gap"

			Width:

				width of the widest button or text is the width of the menu
		*/

		float height = str_list.size() * (text.getCharacterSize() + gap) - gap;

		float width = 0;

		// the text with maximum width

		for (const auto& str : str_list)
		{
			text.setString(str);

			if (width < text.getLocalBounds().width)
			{
				width = text.getLocalBounds().width;
			}
		}

		/*
			get x, y of top center of the menu, which is the top center of the top botton
			since we are center aligning the buttons
		*/

		float x, y;

		bb::to_top_left(x, y, coord.x, coord.y, height, width, pos);

		bb::from_top_left(x, y, x, y, height, width, bb::TOP_CENTER);

		// let's make the button list vector

		std::vector<STR_BUTTON> button_list;

		for (const auto& str : str_list)
		{
			/*
				usually height of the text is smaller than the character size, so we get the
				difference and place the button in the middle of the character size
			*/
		
			text.setString(str);

			auto pad = (text.getCharacterSize() - text.getLocalBounds().height) / 2;

			button_list.push_back(STR_BUTTON(text, str, ordinary_color, hover_color, sf::Vector2f(x, y + pad), bb::TOP_CENTER));

			y += text.getCharacterSize() + gap;
		}

		return bb::MENU<STR_BUTTON>(button_list);
	}
};