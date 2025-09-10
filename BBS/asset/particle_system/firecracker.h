#pragma once

#include<SFML/Graphics.hpp>

#include"../../entity_component_system/entity_component_system.h"


/*
	To create the explosion or firecracker effect,

	declare an object of Firecracker class

	Firecracker explo;

	and call the create() with appropriate arguments to start an explosion

	explo.create(...);

	call update() of 'explo' from Update() and call window.draw(explo) from
	Render() of game loop to display the effect.

	here we have used an ECS to store the particles
	
	internal arrays are used (by ECS) to store the particles, create() creates
	new particles and stores them into ECS and update() deletes each particle
	once their alpha is 0. You can also clear the ECS with clear() and check
	if it is empty or not with empty().
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
			sizeof(sf::Vertex) * m_ecs.component<VERTEX>().capacity() +

			sizeof(sf::Vector2f) * m_ecs.component<START>().capacity() +
			
			sizeof(sf::Vector2f) * m_ecs.component<END>().capacity() +

			sizeof(double) * m_ecs.component<ELAPSED_TIME>().capacity() +

			sizeof(double) * m_ecs.component<DURATION>().capacity() +

			sizeof(double) * m_ecs.component<ALPHA>().capacity() +

			sizeof(double) * m_ecs.component<DALPHA>().capacity()
		);
	}

	size_t sizeInBytes()
	{
		return (sizeof(sf::Vertex) + sizeof(sf::Vector2f) + sizeof(sf::Vector2f) + sizeof(double) + sizeof(double) + sizeof(double) + sizeof(double)) * m_ecs.entity_count();
	}

	/*
		create a new firecracker effect at a new source point

		create simply increases size of internal arrays to fit more vertices
		when all the particles disappear all the arrays are cleared
	*/

	void create(sf::Vector2f source, sf::Color color = sf::Color::White, int count = 1000, double span = 100, double lifeTime = 1)
	{
		// reserve space for new particles

		m_ecs.reserve_extra(count);

		// span represents the diameter so we half it to represent the radius

		span /= 2;

		for (int i = 0; i < count; i++)
		{
			auto&& particle = m_ecs.create_entity();

			particle.get<ALPHA>() = 255;	// initial alpha of each particle

			/*
				total alpha is 255.0, so (255.0 / lifeTime) represents the speed of disappearence
				of the particles

				(255.0 / lifeTime) is the minimum speed, we want some of the particles to disappear
				early so we increase the speed by a random amount, which is also dependent on the
				lifetime, I found 1000 to give best results so I multiply it
			*/

			particle.get<DALPHA>() = 255.0 / lifeTime + (rand() % static_cast<int>(lifeTime * 1000));

			double angle = (rand() % 360) * 3.14 / 180;	// 0 -> 359 degrees but in radians

			// the initial and final positions of the particle

			particle.get<START>() = source;

			/*
				getting random span < actual span and multiplying it with a number between .5 - 1
				to properly randomized the span
			*/

			float rand_span = span * (rand() % 1001) / 1000.0f;	// firecracker

			particle.get<END>() = source + sf::Vector2f(cosf(angle) * rand_span, sinf(angle) * rand_span);

			// elapsed time and duration of the particle

			particle.get<ELAPSED_TIME>() = 0;

			particle.get<DURATION>() = lifeTime;

			// the particle vertex

			particle.get<VERTEX>() = sf::Vertex(source, color);
		}
	}

	void update(double dt)
	{
		for (size_t i = 0; i < m_ecs.entity_count();)
		{
			auto& entity = m_ecs.entity(i);

			auto& alpha = entity.get<ALPHA>();

			// calculating next possible value of alpha of ith particle

			alpha -= entity.get<DALPHA>() * dt;

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

			sf::Vertex& particle = entity.get<VERTEX>();

			sf::Vector2f& start = entity.get<START>();

			sf::Vector2f& end = entity.get<END>();

			double& elapsed_time = entity.get<ELAPSED_TIME>();

			elapsed_time += dt;

			float time_ratio = elapsed_time / entity.get<DURATION>();

			// updating alpha of ith particle

			particle.color.a = static_cast<uint8_t>(alpha);

			// calculating current position
			
			particle.position = start + (end - start) * ((time_ratio >= 1.0f) ? 1.0f : 1.0f - powf(15.0f, -10.0f * time_ratio));

			// adding gravity effect .5 * g * t ^ 2, (.5 * g) = 4, I found this is the best value

			particle.position.y += 14 * elapsed_time * elapsed_time;	// not needed in space explosion

			i++;
		}
	}

private:

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		states.texture = NULL;

		auto& particles = m_ecs.component<VERTEX>();

		if (m_ecs.entity_count() > 0)
		{
			
			target.draw(&particles[0], m_ecs.entity_count(), sf::Points, states);
		}
	}

	mutable bb::ECS<sf::Vertex, sf::Vector2f, sf::Vector2f, double, double, double, double>::C8 m_ecs;

	enum {VERTEX, START, END, ELAPSED_TIME, DURATION, DALPHA, ALPHA};
};