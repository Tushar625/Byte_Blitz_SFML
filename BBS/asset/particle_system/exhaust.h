#pragma once

#include<SFML/Graphics.hpp>

#include"../../entity_component_system/entity_component_system.h"


namespace bb
{
	class Exhaust;
}


class bb::Exhaust : public sf::Drawable
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

	/*
		create a new firecracker effect at a new source point

		create simply increases size of internal arrays to fit more vertices
		when all the particles disappear all the arrays are cleared
	*/

	void spray()
	{
		for (int i = 0; i < m_sprayAmount && m_ecs.entity_count() < m_count; i++)
		{
			auto&& particle = m_ecs.create_entity();

			particle.get<ALPHA>() = 255;	// initial alpha of each particle

			/*
				total alpha is 255.0, (255.0 / lifeTime) represents the rate of disappearence
				of a particle

				"lifeTime" is (m_span / m_maxVelocity), here, "m_span" is the distance traversed by
				the particles and "m_maxVelocity" is the maximum possible velocity, so "lifeTime" is
				the minimum time required to traverse "m_span".

				here, we introduce randomness by replacing "m_span" with "(rand() % m_span + 1.0)"

				dalpha and "m_span" are inversely proportional, by lowering "m_span" we increase dalpha

				min dalpha = 255.0 / (m_span / m_maxVelocity)

				max dalpha = 255.0 / (1.0 / m_maxVelocity)

				for the minimum value of dalpha the particle will fade over "m_span"
				distance
			*/

			particle.get<DALPHA>() = 255.0 / ((rand() % m_span + 1.0) / m_maxVelocity);

			// select a random angle between [(m_direction - m_angle / 2) - (m_direction + m_angle / 2)]

			double angle = (rand() % (m_angle + 1) - m_angle / 2 + m_direction) * 3.14 / 180;

			sf::Vector2f trigo = { cosf(angle), sinf(angle) };	// calcuate cos and sin of the angle in advance

			// velocity of the particle in the range [0 - (m_maxVelocity - 1)]

			int velo = rand() % m_maxVelocity;

			// getting components of the velocity

			particle.get<VELOCITY>() = sf::Vector2f(trigo.x * velo, trigo.y * velo);

			// we push the particles further from source point to better fit the nozzle

			particle.get<VERTEX>() = sf::Vertex(sf::Vector2f{m_source.x + trigo.x * m_gap, m_source.y + trigo.y * m_gap}, m_color);
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

			if (alpha <= 0)
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

			auto& particle = entity.get<VERTEX>();

			// updating alpha of ith particle

			particle.color.a = static_cast<uint8_t>(alpha);

			// updating position based on velociity

			auto& velocity = entity.get<VELOCITY>();

			particle.position.x += static_cast<float>(velocity.x * dt);

			particle.position.y += static_cast<float>(velocity.y * dt);

			i++;
		}
	}

	// consructor

	Exhaust() :
		m_source{ 100, 100 },
		m_direction(0),
		m_angle(20),

		m_color{ sf::Color::White },
		m_count(10000),
		m_sprayAmount(100),
		m_span(50),
		m_gap(0),
		m_maxVelocity(100)
	{
		m_ecs.reserve_extra(m_count);
	}

	// getters and setters

	sf::Vector2f getSource() const noexcept
	{
		return m_source;
	}

	void setSource(const sf::Vector2f &pt) noexcept
	{
		m_source = pt;
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

	mutable bb::ECS<sf::Vertex, double, double, sf::Vector2f>::C8 m_ecs;

	enum { VERTEX, DALPHA, ALPHA, VELOCITY };

	
	sf::Vector2f m_source;

	double m_direction;

	uint16_t m_angle;

	
	sf::Color m_color;

	uint32_t m_count;

	uint16_t m_sprayAmount;

	uint16_t m_span;

	uint8_t m_gap;

	uint32_t m_maxVelocity;
};