
/*
	this header file provides two predefined objects of this game engine
	to take user input

	here we detect only key press and current position of mouse pointer on
	the window
*/


#pragma once


#include"window.h"


class InputClass
{
	sf::Keyboard::Scan::Scancode m_key;	// the pressed or released key

	bool m_keyState;	// whether key is pressed or released

	sf::Mouse::Button m_button;	// the pressed or released mouse button

	bool m_buttonState;	// whether button is pressed or released

	sf::Event m_event;

	bool m_close;	// to detect the window close event

public:

	InputClass()
	{
		// by default repeated key inputs are disabled

		WINDOW.setKeyRepeatEnabled(false);
	}

	// look for events, this function must be called in each iteration of game loop

	void scan()
	{
		// initializing the member variables to cleanup their previous values

		m_close = false;

		m_key = sf::Keyboard::Scan::ScancodeCount;

		m_button = sf::Mouse::ButtonCount;

		while (WINDOW.pollEvent(m_event))
		{
			switch (m_event.type)
			{
			case sf::Event::Closed:

				m_close = true;

				break;

			case sf::Event::KeyPressed:

				// getting only one key, forget the rest

				m_key = m_event.key.scancode;

				m_keyState = true;

				break;

			case sf::Event::KeyReleased:

				// getting only one key, forget the rest

				m_key = m_event.key.scancode;

				m_keyState = false;

				break;

			case sf::Event::MouseButtonPressed:

				// getting only one button, forget the rest

				m_button = m_event.mouseButton.button;

				m_buttonState = true;

				break;

			case sf::Event::MouseButtonReleased:

				// getting only one button, forget the rest

				m_button = m_event.mouseButton.button;

				m_buttonState = false;

				break;
			}
		}
	}

	// this function returns true when the close button is clicked

	bool isClosed()
	{
		return m_close;
	}

	// returns true if the key is just pressed else 0

	bool isPressed(sf::Keyboard::Scan::Scancode key)
	{
		return m_key == key && m_keyState == true;
	}

	bool isPressedM(sf::Mouse::Button button)
	{
		return m_button == button && m_buttonState == true;
	}

	// returns true if the key is pressed and held down else 0

	bool isHeld(sf::Keyboard::Scan::Scancode key)
	{
		return WINDOW.hasFocus() && sf::Keyboard::isKeyPressed(key);
	}

	bool isHeldM(sf::Mouse::Button button)
	{
		return WINDOW.hasFocus() && sf::Mouse::isButtonPressed(button);
	}

	// returns true if the key is just released else 0

	bool isReleased(sf::Keyboard::Scan::Scancode key)
	{
		return m_key == key && m_keyState == false;
	}

	bool isReleasedM(sf::Mouse::Button button)
	{
		return m_button == button && m_buttonState == false;
	}

	sf::Vector2i pointer()
	{
		return sf::Mouse::getPosition(WINDOW);
	}

} INPUT;