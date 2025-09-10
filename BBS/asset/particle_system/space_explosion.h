#pragma once

#include<SFML/Graphics.hpp>

#include"../../entity_component_system/entity_component_system.h"

#include"../../utility/pos_fun.h"


/*
	To create the explosion in space effect,

	declare an object of SpaceExplosion class

	SpaceExplosion explo;

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
	class SpaceExplosion;
}


class bb::SpaceExplosion : public sf::Drawable
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
		create a new space explosion effect at a new source point, you can also input
		velocity of the exploding object.

		create simply increases size of internal arrays to fit more vertices
		when all the particles disappear all the arrays are cleared
	*/

	void create(sf::Vector2f source, sf::Vector2f source_velocity, sf::Color color = sf::Color::White, int count = 1000, double span = 100, double lifeTime = 1)
	{
		// reserve space for new particles

		m_ecs.reserve_extra(count);

		// span represents the diameter so we half it to represent the radius

		span /= 2;

		// source velocity magnitude and direction in degrees

		float velocity_m = 0, velocity_d = 0;	// if source velocity is 0 magnitude and direction remains 0

		if(source_velocity != sf::Vector2f(0, 0))
		{
			velocity_m = 1 / fast_inv_sqrt(source_velocity.x * source_velocity.x + source_velocity.y * source_velocity.y);

			velocity_d = atan(source_velocity.y / source_velocity.x) * 180 / 3.14;

			if (source_velocity.x < 0)
			{
				velocity_d += 180;
			}
		}

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

			float rand_span = rand() % int(span) * (rand() % 501 + 500) / 1000.0f;	// space explosion better

			/*
				getting a random direction, which is between source velocity direction - 10 and
				source velocity direction + 10
			*/

			float rand_velocity_d = (velocity_d + rand() % 21 - 10) * 3.14 / 180;	// -10 -> +10

			/*
				random fraction of the magnitude of source velocity
			*/

			float rand_velocity_m = velocity_m * (rand() % 501 + 500) / 20000.0f;	// 0.017 - 0.033

			/*
				now we combine the random direction and random magnitude to create a
				random velocity vector, that will influence this particle
			*/

			sf::Vector2f rand_velocity = sf::Vector2f(rand_velocity_m * cosf(rand_velocity_d), rand_velocity_m * sinf(rand_velocity_d));

			/*
				source + sf::Vector2f(cosf(angle) * rand_span, sinf(angle) * rand_span) : the final position

				rand_velocity * (float)lifeTime: influence of source velocity
			*/

			particle.get<END>() = source + sf::Vector2f(cosf(angle) * rand_span, sinf(angle) * rand_span) + rand_velocity * (float)lifeTime;

			// elapsed time and duration of the particle

			particle.get<ELAPSED_TIME>() = 0;

			particle.get<DURATION>() = lifeTime;

			// the particle vertex

			particle.get<VERTEX>() = sf::Vertex(source, color);
		}
	}

	/*
		create a new space explosion effect with hollow center at a new source point, you can also input
		velocity of the exploding object.

		create simply increases size of internal arrays to fit more vertices
		when all the particles disappear all the arrays are cleared
	*/

	void createHollow(sf::Vector2f source, sf::Vector2f source_velocity, sf::Color color = sf::Color::White, int count = 1000, double span = 100, double lifeTime = 1)
	{
		// reserve space for new particles

		m_ecs.reserve_extra(count);

		// span represents the diameter so we half it to represent the radius

		span /= 2;

		// source velocity magnitude and direction in degrees

		float velocity_m = 0, velocity_d = 0;	// if source velocity is 0 magnitude and direction remains 0

		if (source_velocity != sf::Vector2f(0, 0))
		{
			velocity_m = 1 / fast_inv_sqrt(source_velocity.x * source_velocity.x + source_velocity.y * source_velocity.y);

			velocity_d = atan(source_velocity.y / source_velocity.x) * 180 / 3.14;

			if (source_velocity.x < 0)
			{
				velocity_d += 180;
			}
		}

		float hollow_offset = 3.14 / (rand() % 4 + 1);	// determines the orientation of hollow pattern

		int hollow_range = rand() % 4 + 6;	// determines the shape of hollow pattern

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

			float rand_span = rand() % int(span) * (rand() % 501 + 500) / 1000.0f;	// space explosion better

			/*
				getting a random direction, which is between source velocity direction - 10 and
				source velocity direction + 10
			*/

			float rand_velocity_d = (velocity_d + rand() % 21 - 10) * 3.14 / 180;	// -10 -> +10

			/*
				random fraction of the magnitude of source velocity
			*/

			float rand_velocity_m = velocity_m * (rand() % 501 + 500) / 20000.0f;	// 0.025 - 0.05

			/*
				now we combine the random direction and random magnitude to create a
				random velocity vector, that will influence this particle
			*/

			sf::Vector2f rand_velocity = sf::Vector2f(rand_velocity_m * cosf(rand_velocity_d), rand_velocity_m * sinf(rand_velocity_d));

			/*
				hollow factor is used to make the center of the explosion hollow with
				a star like shape around it

				the shape of that pattern is determined by "hollow_range", which is used
				to generate a random number between -hollow_range and +hollow_range

				"hollow_offset" rotates the pattern, so that 2 patterns with same "hollow_range"
				looks different
			*/

			float hollow_factor = (rand() % (hollow_range * 2 + 1) - hollow_range) + hollow_offset;

			/*
				hollow_velocity is a 2D vector to give the particle some extra speed required
				to create the hollow effect
			*/

			sf::Vector2f hollow_velocity = sf::Vector2f(cosf(hollow_factor), sinf(hollow_factor));

			/*
				source + sf::Vector2f(cosf(angle) * rand_span, sinf(angle) * rand_span) : the final position

				rand_velocity * (float)lifeTime: influence of source velocity

				hollow_velocity * float(span * 2) / float(rand() % 3 + 9): influence of hollow effect

				(float(span * 2) / float(rand() % 3 + 9)) determines the size of hollow effect, it depends on span

				float(rand() % 3 + 9) is to make the pattern blurry
			*/

			particle.get<END>() = source + sf::Vector2f(cosf(angle) * rand_span, sinf(angle) * rand_span) + rand_velocity * (float)lifeTime + hollow_velocity * float(span * 2) / float(rand() % 3 + 9);

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

			// subtracted from 1 to slow down the particles as the time progresses

			float time_ratio = 1 - elapsed_time / entity.get<DURATION>();

			// updating alpha of ith particle

			particle.color.a = static_cast<uint8_t>(alpha);

			// calculating current position

			particle.position = start + (end - start) * (1 - time_ratio * time_ratio * time_ratio * time_ratio * time_ratio * time_ratio * time_ratio * time_ratio);

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

	enum { VERTEX, START, END, ELAPSED_TIME, DURATION, DALPHA, ALPHA };
};