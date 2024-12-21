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
		m_particles.clear();

		m_dAlpha.clear();

		m_Alpha.clear();

		m_velocity.clear();
	}

	bool empty()
	{
		return m_particles.empty() && m_Alpha.empty() && m_dAlpha.empty() && m_velocity.empty();
	}

	// total size reserved in bytes

	size_t capacityInBytes()
	{
		return (sizeof(sf::Vertex) * m_particles.capacity() + sizeof(double) * m_Alpha.capacity() + sizeof(double) * m_dAlpha.capacity() + sizeof(sf::Vector2f) * m_velocity.capacity());
	}

	size_t sizeInBytes()
	{
		return (sizeof(sf::Vertex) * m_particles.size() + sizeof(double) * m_Alpha.size() + sizeof(double) * m_dAlpha.size() + sizeof(sf::Vector2f) * m_velocity.size());
	}

	/*
		create a new firecracker effect at a new source point

		create simply increases size of internal arrays to fit more vertices
		when all the particles disappear all the arrays are cleared
	*/

	void create(sf::Vector2f source, sf::Color color = sf::Color::White, int count = 1000, double span = 100, double lifeTime = 1)
	{
		// reserve space for new particles

		m_particles.reserve(m_particles.size() + count);

		m_Alpha.reserve(m_Alpha.size() + count);
		
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
		for (unsigned i = 0; i < m_particles.size();)
		{
			// calculating next possible value of alpha of ith particle

			m_Alpha[i] -= m_dAlpha[i] * dt;

			if(m_Alpha[i] <= 0)
			{
				/*
					if alpha of a point <= 0 the point is not visible so we "delete" it by,
					replacing the point with last point and then deleting the last point
				*/
			
				auto endIndex = m_particles.size() - 1;

				m_particles[i] = m_particles[endIndex];

				m_dAlpha[i] = m_dAlpha[endIndex];

				m_Alpha[i] = m_Alpha[endIndex];

				m_velocity[i] = m_velocity[endIndex];

				m_particles.pop_back();

				m_dAlpha.pop_back();
				
				m_Alpha.pop_back();
				
				m_velocity.pop_back();

				// now go to the top of this iteration to update this new particle's alpha

				// if the deleted particle was the last particle the loop condition will stop the loop

				continue;
			}

			sf::Vector2f accn; 
			
			sf::Vector2f& velocity = m_velocity[i];

			sf::Vertex& particle = m_particles[i];

			// updating alpha of ith particle

			particle.color.a = static_cast<uint8_t>(m_Alpha[i]);

			// calculating accn from velocity to simulate drag

			accn.x = -25 * velocity.x;

			accn.y = -25 * velocity.y;

			velocity.x += static_cast<float>(accn.x * dt);

			velocity.y += static_cast<float>(accn.y * dt + 200 * dt);	// adding downward accn to simulate gravity

			particle.position.x += static_cast<float>(velocity.x * dt);

			particle.position.y += static_cast<float>(velocity.y * dt);

			i++;
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