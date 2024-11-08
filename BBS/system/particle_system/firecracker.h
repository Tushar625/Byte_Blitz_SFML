#pragma once

#include<SFML/Graphics.hpp>

#include<vector>


namespace bb
{
	class Firecracker;
}


class bb::Firecracker : public sf::Drawable
{
public:

	/*
		create a new perticle effect at a new source point

		create simply increases size of internal arrays to fit more vertices
		when all the perticles disappear all the arrays are cleared
	*/

	void create(sf::Vector2f source, sf::Color color = sf::Color::White, int count = 1000, double span = 100, double lifeTime = 1)
	{
		// reserve space for new vertices

		m_particles.reserve(m_particles.size() + count);

		m_dAlpha.reserve(m_dAlpha.size() + count);

		m_velocity.reserve(m_velocity.size() + count);

		// span represents the diameter so we half it to represent the radius

		span /= 2;

		for (int i = 0; i < count; i++)
		{
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

			m_velocity.push_back(sf::Vector2f(sin(angle) * velo, cos(angle) * velo));

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
			// calculating next possible value of alpha

			int alpha = m_particles[i].color.a - m_dAlpha[i] * dt;

			if (alpha <= 0)
			{
				continue;	// no need to upgrade if alpha is 0
			}

			// calculating accn according to velocity to simulate drag

			sf::Vector2f accn; 
			
			sf::Vector2f &velocity = m_velocity[i];

			sf::Vertex& particle = m_particles[i];

			// calculating accn from velocity to simulate drag

			accn.x = -25 * velocity.x;

			accn.y = -25 * velocity.y;

			velocity.x += accn.x * dt;

			velocity.y += accn.y * dt + 200 * dt;	// adding downward accn to simulate gravity

			particle.position.x += velocity.x * dt;

			particle.position.y += velocity.y * dt;

			particle.color.a = (alpha <= 0) ? 0 : alpha;

			flag = false;
		}

		if (flag)
		{
			// every one has alpha 0

			m_particles.clear();

			m_dAlpha.clear();

			m_velocity.clear();
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

	std::vector<sf::Vector2f> m_velocity;
};