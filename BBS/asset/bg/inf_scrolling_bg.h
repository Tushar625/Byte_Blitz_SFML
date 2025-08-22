#pragma once

#include<SFML/Graphics.hpp>

#include"../../system/window.h"



namespace bb
{	
	class InfScrollingBG;
}

/*
	this class creates an infinite scrolling background effect

	how to use:

		1. prepare a sprite to be used as the background
		   (make sure that the sprite can be repeated seamlessly to create the infinite scrolling effect)
	
		2. create an object of this class, passing the required parameters

			InfScrollingBG bg(bgSprite, xRepeat, speed, screenHeight);

			where,

				bgSprite     : the sprite to be used as the background
				
				xRepeat      : starting from the front, the x position in the sprite where
						       the texture repeats itself, i.e., same as the front
				
				speed        : speed of the bg moving right to left

				screenHeight : height of the screen where the bg is to be rendered. bg sprite
							   will be scaled to fit this screen height

		3. call update(dt) in the update function of your game loop update()

		4. call render() in the render function of your game loop render

		5. you can change the speed of the bg using setSpeed(speed)
*/



class bb::InfScrollingBG
{

	sf::Sprite m_bgSprite;	// the background sprite

	float m_x;				// current x position of the bg sprite
	
	float m_scale;			// scaling factor of the bg sprite to fit the screen height
	
	float m_speed;			// speed of the bg moving right to left
	
	float m_xRepeat;		// at this point we reset m_x to 0, to create the infinite scrolling animation


public:


	/*
		the constructor requires:
			1. the sprite to be used as the background
			2. xRepeat: starting from the front, the x position in the sprite where
			   the texture repeats itself, i.e., same as the front
			3. speed: speed of the bg moving right to left
			4. screenHeight: height of the screen where the bg is to be rendered. bg
			   sprite will be scaled to fit this screen height
	*/
	
	InfScrollingBG(const sf::Sprite& bgSprite, float xRepeat, float speed, float screenHeight) :
		m_bgSprite(bgSprite),
		m_x(0),
		m_scale(screenHeight / bgSprite.getGlobalBounds().height),
		m_speed(speed),
		m_xRepeat(xRepeat* m_scale)	// scaling xRepeat as we scale the bg sprite
	{
		// scaling bgSprite in a way that fits entire height of the picture in the screen

		m_bgSprite.setScale(m_scale, m_scale);
	}


	// the setters


	void setSpeed(float speed) noexcept
	{
		m_speed = speed;
	}


	// the getters


	float getSpeed() const noexcept
	{
		return m_speed;
	}


	void update(double dt) noexcept
	{
		m_x -= m_speed * dt;	// moving bg right to left

		if (m_x < -m_xRepeat)
		{
			/*
				after certain position we set bg to it's x = 0 as this "position"
				is identical to the front of this sprite, this creates the looping
				bg effect
			*/

			m_x = 0;
		}
	}


	void render() noexcept
	{
		m_bgSprite.setPosition(m_x, 0);

		bb::WINDOW.draw(m_bgSprite);
	}
};