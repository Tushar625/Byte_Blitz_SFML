#pragma once

#include<SFML/Graphics.hpp>
#include<iostream>
#include"../../entity_component_system/entity_component_system_packed.h"


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
		m_ecs.clear();
	}

	bool empty()
	{
		return m_ecs.empty();
	}

	// total size reserved in bytes

	size_t capacityInBytes()
	{
		return (
			sizeof(t_vertex) * m_ecs.component<t_vertex>(VERTEX).capacity() +
			sizeof(t_alpha) * m_ecs.component<t_alpha>(ALPHA).capacity() +
			sizeof(t_dalpha) * m_ecs.component<t_dalpha>(DALPHA).capacity() +
			sizeof(t_velocity) * m_ecs.component<t_velocity>(VELOCITY).capacity()
		);
	}

	size_t sizeInBytes()
	{
		return (sizeof(t_vertex) + sizeof(t_alpha) + sizeof(t_dalpha) + sizeof(t_velocity)) * m_ecs.entity_count();
	}

	/*
		create a new firecracker effect at a new source point

		create simply increases size of internal arrays to fit more vertices
		when all the particles disappear all the arrays are cleared
	*/

	void create(sf::Vector2f source, sf::Color color = sf::Color::White, int count = 1000, double span = 100, double lifeTime = 1)
	{
		ENTITY_COMPONENT_SYSTEM_PACKED<t_vertex, t_dalpha, t_alpha, t_velocity>::ENTITY particle(m_ecs);

		// reserve space for new particles

		m_ecs.reserve_extra(count);

		// span represents the diameter so we half it to represent the radius

		span /= 2;

		for (int i = 0; i < count; i++)
		{
			particle.id = m_ecs.create_entity().id;

			particle.get<t_alpha>(ALPHA) = 255;	// initial alpha of each particle

			/*
				total alpha is 255.0, so (255.0 / lifeTime) represents the speed of disappearence
				of the particles

				(255.0 / lifeTime) is the minimum speed, we want some of the particles to disappear
				early so we increase the speed by a random amount, which is also dependent on the
				lifetime, I found 1000 to give best results so I multiply it
			*/

			particle.get<t_dalpha>(DALPHA) = 255.0 / lifeTime + (rand() % static_cast<int>(lifeTime * 1000));

			double angle = (rand() % 360) * 3.14 / 180;	// 0 -> 359 degrees but in radians

			/*
				the particles will be visible for "lifeTime" seconds and will traverse "span"
				pixels so I thought of using "span / lifeTime" but the particles have a -ve acceleration
				to slow them dowm so I square the "span / lifeTime" and multiply .35, this gives the
				best results
			*/

			int velo = rand() % static_cast<int>(span * span / lifeTime * lifeTime * .35);

			// getting components of the velocity

			particle.get<t_velocity>(VELOCITY) = sf::Vector2f(static_cast<float>(sin(angle) * velo), static_cast<float>(cos(angle) * velo));

			particle.get<t_vertex>(VERTEX) = sf::Vertex(source, color);
		}
	}

	void update(double dt)
	{
		for (size_t i = 0; i < m_ecs.entity_count();)
		{
			auto& entity = m_ecs.entity(i);

			auto& alpha = entity.get<t_alpha>(ALPHA);

			// calculating next possible value of alpha of ith particle

			alpha -= entity.get<t_dalpha>(DALPHA) * dt;

			/*if (i == 0)
			{
				std::cout << m_ecs.index_of_compoent<t_vertex>() << ' ' << m_ecs.index_of_compoent< t_alpha > () << ' ' << m_ecs.index_of_compoent<t_dalpha>() << ' ' << m_ecs.index_of_compoent<t_velocity>() << '\n';
			}*/

			if(alpha <= 0)
			{
				/*
					if alpha of a point <= 0 the point is not visible so we "delete" it by,
					replacing the point with last point and then deleting the last point
				*/
			
				m_ecs.kill_entity(entity);

				// now go to the top of this iteration to update this new particle's alpha

				// if the deleted particle was the last particle the loop condition will stop the loop

				continue;
			}

			sf::Vector2f accn; 
			
			sf::Vector2f& velocity = entity.get<t_velocity>(VELOCITY);

			sf::Vertex& particle = entity.get<t_vertex>(VERTEX);

			// updating alpha of ith particle

			particle.color.a = static_cast<uint8_t>(alpha);

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

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		states.texture = NULL;

		auto& particles = m_ecs.component<t_vertex>(VERTEX);

		if (m_ecs.entity_count() > 0)
		{
			
			target.draw(&particles[0], m_ecs.entity_count(), sf::Points, states);
		}
	}

	using t_vertex = sf::Vertex;

	using t_dalpha = double;

	using t_alpha = float;

	using t_velocity = sf::Vector2f;

	mutable ENTITY_COMPONENT_SYSTEM_PACKED<t_vertex, t_dalpha, t_alpha, t_velocity> m_ecs;

	enum {VERTEX, DALPHA, ALPHA, VELOCITY};
};