#pragma once

#include <SFML/Graphics.hpp>

#include <functional>

#include "../../timer/timer.h"

#include "../../system/window.h"



namespace bb
{
	class ScreenFade;
}



/*
	this class is used to create a fade in and fade out effect on the screen

	it can cover the whole screen or a part of it with a rectangle and tweens its alpha value
	between 0 and 255 to create the fade effect

	Example usage:

	ScreenFade screenFade(sf::Vector2f(800, 600));	// create a screen fade object with the size of the window

	// you can set
	// => the color of the fade effect, default is white
	// => the duration of the fade effect, default is 1 second
	// => chage the position and size of the fade rectangle, default position is (0, 0)
	//    and size is the one set by constructor

	// after setting the properties, you can start the fade effect

	// for example, to start a fade in effect, where the screen emarges from darkness

	screenFade.setColor(sf::Color::Black);	// set the color of the fade effect to black

	screenFade.startFadeIn([]() { ... });	// you can provide an optional final function that
											// will be called after the fade in effect is over

	// similarly, to start a fade out effect, where the screen goes white slowly

	screenFade.setColor(sf::Color::White);	// set the color of the fade effect to white

	screenFade.startFadeOut([]() { ... });	// you can provide an optional final function that
											// will be called after the fade out effect is over

	// to execute the final function after the fade in effect is over, you must call xfinal() in the update()

	if (screenFade.xfinal())
	{
		// xfinal() returns true only when the final() function is executed
	}

	// if you are accepting inputs in update(), you might want to disable input
	// while the fade effect is running, use isActive() to check if the fade
	// effect is running or not

	screenFade.isActive();	// returns true if the fade effect is running, false otherwise

	// to stop the fade effect abruptly, without calling final function, use
	// stop() method, it stops the internal tweening thread and makes the fade effect inactive

	screenFade.stop();	// this will stop the fade effect immediately

	// to render the fade effect on the screen, simply call render() in the render()

	screenFade.render();	// this will render the fade effect on the screen if it is active
*/



class bb::ScreenFade
{
	sf::RectangleShape m_screen;	// the rectangle that covers the whole screen for fade effect

	sf::Color m_color;	// the color of the fade effect


	double m_duration;	// how long it takes to fade in or out completely, in seconds

	uint8_t m_alpha;	// alpha of m_screen rectangle, it's tweened between 0 and 255, to implement the fade effect

	bool m_active;	// indicates if the fade effect is running or not


	bb::TWEENER m_tween;



public:



	explicit ScreenFade(const sf::Vector2f& size = { 800, 600 }) :
		m_screen(size),
		m_color(sf::Color::White),	// default color of fade effect is white
		m_duration(1.0),	// default duration of fade effect is 1 second
		m_alpha(0),
		m_active(false)
	{}



	// the setters



	void setColor(const sf::Color& newColor) noexcept
	{
		m_color = newColor;
	}


	void setDuration(double duration) noexcept
	{
		m_duration = duration;
	}


	void setPosition(const sf::Vector2f& position) noexcept
	{
		m_screen.setPosition(position);
	}


	void setSize(const sf::Vector2f& size) noexcept
	{
		m_screen.setSize(size);
	}



	// the getters



	sf::Color getColor() const noexcept
	{
		return m_color;
	}


	double getDuration() const noexcept
	{
		return m_duration;
	}


	sf::Vector2f getPosition() const noexcept
	{
		return m_screen.getPosition();
	}


	sf::Vector2f getSize() const noexcept
	{
		return m_screen.getSize();
	}



	// operational methods



	// check if the fade effect is running or not

	bool isActive() const noexcept
	{
		return m_active;
	}


	/*
		start the fade in effect, i.e., alpha goes from 255 to 0
		
		if final() is provided, it will be called after the fade in effect is over
	*/

	bool startFadeIn(std::function<void(void)> final = nullptr) noexcept
	{
		if(m_active)
		{
			return false;	// already active
		}

		m_active = true;

		m_tween.start(
			m_duration,
			bb::twn(m_alpha, (uint8_t)255, (uint8_t)0),
			[this, final](double dt)
			{
				m_active = false;

				if (final)
				{
					final();
				}
			}
		);

		return true;
	}


	/*
		start the fade out effect, i.e., alpha goes from 255 to 0

		if final() is provided, it will be called after the fade out effect is over
	*/

	bool startFadeOut(std::function<void(void)> final = nullptr) noexcept
	{
		if (m_active)
		{
			return false;	// already active
		}

		m_active = true;

		m_tween.start(
			m_duration,
			bb::twn(m_alpha, (uint8_t)0, (uint8_t)255),
			[this, final](double dt)
			{
				m_active = false;

				if (final)
				{
					final();
				}
			}
		);

		return true;
	}


	/*
		execute the final() function after the fade in / out effect is over

		returns true only when final() is executed after the fade in / out effect is over
	*/

	bool xfinal() noexcept
	{
		return m_tween.xfinal();
	}


	// stop the fade effect immediately, without calling final() function

	void stop() noexcept
	{
		m_active = false;

		m_tween.stop();
	}


	// render the fade effect on the screen when it is active

	void render() noexcept
	{
		if (m_active)
		{
			m_tween.lock();

			m_screen.setFillColor(sf::Color(m_color.r, m_color.g, m_color.b, m_alpha));

			m_tween.unlock();

			bb::WINDOW.draw(m_screen);
		}
	}
};