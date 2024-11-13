#pragma once

#include<SFML/Graphics.hpp>

#include<vector>


/*
	To create the explosion or firecracker effect,

	declare an object of Firecracker class

	Firecracker explo;

	and call the create() with appropriate arguments to start an explosion

	explo.create(...);

	call update() and render() of 'explo' from Update() and Render() of game
	loop to display the effect.

	internal arrays are used to store the particles, create() creates those
	arrays and update() autometically clears them once the show is over, you
	can also clear them with clear() and check if they are empty or not with
	empty()
*/


namespace bb
{
	class Firecracker;
}


class bb::Firecracker : public sf::Drawable
{
public:

	void clear()
	{
		// I do this to make sure that the memory gets deallocated

		std::vector<sf::Vertex>().swap(m_particles);

		std::vector<double>().swap(m_dAlpha);

		std::vector<double>().swap(m_Alpha);

		std::vector<sf::Vector2f>().swap(m_velocity);
	}

	bool empty()
	{
		return m_particles.empty() && m_Alpha.empty() && m_dAlpha.empty() && m_velocity.empty();
	}

	/*
		create a new perticle effect at a new source point

		create simply increases size of internal arrays to fit more vertices
		when all the perticles disappear all the arrays are cleared
	*/

	void create(sf::Vector2f source, sf::Color color = sf::Color::White, int count = 1000, double span = 100, double lifeTime = 1)
	{
		// reserve space for new particles

		m_particles.reserve(m_particles.size() + count);

		m_dAlpha.reserve(m_Alpha.size() + count);
		
		m_dAlpha.reserve(m_dAlpha.size() + count);

		m_velocity.reserve(m_velocity.size() + count);

		// span represents the diameter so we half it to represent the radius

		span /= 2;

		for (int i = 0; i < count; i++)
		{
			m_Alpha.push_back(255);	// initial alpha of each particle

			/*
				total alpha is 255.0, so (255.0 / lifeTime) represents the speed of disappearence
				of the particles

				(255.0 / lifeTime) is the minimum speed, we want some of the particles to disappear
				early so we increase the speed by a random amount, which is also dependent on the
				lifetime, I found 1000 to give best results so I multiply it
			*/

			m_dAlpha.push_back(255.0 / lifeTime + (rand() % static_cast<int>(lifeTime * 1000)));

			double angle = (rand() % 360) * 3.14 / 180;	// 0 -> 359 degrees but in radians

			/*
				the particles will be visible for "lifeTime" seconds and will traverse "span"
				pixels so I thought of using "span / lifeTime" but the particles have a -ve acceleration
				to slow them dowm so I square the "span / lifeTime" and multiply .35, this gives the
				best results
			*/

			int velo = rand() % static_cast<int>(span * span / lifeTime * lifeTime * .35);

			// getting components of the velocity

			m_velocity.push_back(sf::Vector2f(static_cast<float>(sin(angle) * velo), static_cast<float>(cos(angle) * velo)));

			m_particles.push_back(sf::Vertex(source, color));
		}
	}

	void update(double dt)
	{
		if (m_particles.size() <= 0)
		{
			return;
		}

		bool flag = true;

		for (int i = 0; i < m_particles.size(); i++)
		{
			// calculating accn according to velocity to simulate drag

			sf::Vector2f accn; 
			
			sf::Vector2f &velocity = m_velocity[i];

			sf::Vertex& particle = m_particles[i];

			// calculating accn from velocity to simulate drag

			accn.x = -25 * velocity.x;

			accn.y = -25 * velocity.y;

			velocity.x += static_cast<float>(accn.x * dt);

			velocity.y += static_cast<float>(accn.y * dt + 200 * dt);	// adding downward accn to simulate gravity

			particle.position.x += static_cast<float>(velocity.x * dt);

			particle.position.y += static_cast<float>(velocity.y * dt);

			// calculating next possible value of alpha

			m_Alpha[i] -= m_dAlpha[i] * dt;

			particle.color.a = (m_Alpha[i] <= 0) ? 0 : (flag = false, static_cast<uint8_t>(m_Alpha[i]));

			// when all the alpha becomes 0 or less than 0 the flag never set to false and the memeory is deallocated
		}

		if (flag)
		{
			// every one has alpha 0

			clear();
		}
	}

private:

	void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.texture = NULL;

		if (m_particles.size() > 0)
		{
			target.draw(&m_particles[0], m_particles.size(), sf::Points, states);
		}
	}

	std::vector<sf::Vertex> m_particles;

	std::vector<double> m_dAlpha;

	std::vector<double> m_Alpha;

	std::vector<sf::Vector2f> m_velocity;
};