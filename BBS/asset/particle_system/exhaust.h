#pragma once

#include<SFML/Graphics.hpp>

#include"../../entity_component_system/entity_component_system.h"


namespace bb
{
	class Exhaust;
}


/*
	To create the exhaust animation,

	declare an object of Exhaust class

	Exhaust exhaust;
	
	or 
	
	Exhaust exhaust(1000);	// 1000 is the total no. of particles used for the simulation
							// by default 10000 particles are used

	customize the effect by setting the source point, direction, angle, color, spray amount,
	span, gap and max velocity of the particles with the getter and setter functions, defined
	in the class

	and call the spray() to "spray" certain no. of particles, as set with setSprayAmount()
	method, call it repeatedly to create the exhaust effect

	exhaust.spray();	// doesn't return take any arguments

	call update(dt) of 'exhaust' from Update() and call window.draw(exhaust) from
	Render() of game loop to display the effect.

	you can also delete all the particles with exhaust.clear()
*/


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
		sprays <= "m_sprayAmount" no. of particles, call this function repeatedly
		to get a consistent spray of particles

		it also recycles the deleted particles
	*/

	void spray() noexcept
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

			// getting components of the velocity according to "angle"

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

	explicit Exhaust(uint32_t count = DEFAULT_COUNT) :
		m_source{ 0, 0 },
		m_direction(DEFAULT_DIRECTION),
		m_angle(DEFAULT_ANGLE),

		m_color{ sf::Color::White },
		m_count(count),
		m_sprayAmount(DEFAULT_SPRAY_AMOUNT),
		m_span(DEFAULT_SPAN),
		m_gap(DEFAULT_GAP),
		m_maxVelocity(DEFAULT_MAX_VELOCITY)
	{
		// reserve space for all the particles

		m_ecs.reserve_extra(m_count);
	}

	/*
		getters and setters:

		use them to customize the exhaust animation

		notes:

		set direction and angle data in degrees

		if you increase the max velocity and the animation becomes bad
		increase the span make it more than 50% of the max velocity

		spray amount should be around 10% of total particle count else
		the animation starts off with a pulse

		These numbers are just some estimates, you may need to fiddle
		with these parameter a bit to get the perfect exhaust animation

		better don't change count, spray amount and maximum valocity
	*/

	// source

	sf::Vector2f getSource() const noexcept
	{
		return m_source;
	}

	void setSource(const sf::Vector2f &pt = {0, 0}) noexcept
	{
		m_source = pt;
	}

	// direction

	double getDirection() const noexcept
	{
		return m_direction;
	}

	void setDirection(double direction = DEFAULT_DIRECTION) noexcept
	{
		m_direction = direction;
	}

	// angle

	uint16_t getAngle() const noexcept
	{
		return m_angle;
	}

	void setAngle(uint16_t angle = DEFAULT_ANGLE) noexcept
	{
		m_angle = angle;
	}

	// color

	sf::Color getColor() const noexcept
	{
		return m_color;
	}

	void setColor(sf::Color color = sf::Color::White) noexcept
	{
		m_color = color;
	}

	// spray amount

	uint16_t getSprayAmount() const noexcept
	{
		return m_sprayAmount;
	}

	void setSprayAmount(uint16_t sprayAmount = DEFAULT_SPRAY_AMOUNT) noexcept
	{
		m_sprayAmount = sprayAmount;
	}

	// span

	uint16_t getSpan() const noexcept
	{
		return m_span;
	}

	void setSpan(uint16_t span = DEFAULT_SPAN) noexcept
	{
		m_span = span;
	}

	// gap

	uint8_t getGap() const noexcept
	{
		return m_gap;
	}

	void setGap(uint8_t gap = DEFAULT_GAP) noexcept
	{
		m_gap = gap;
	}

	// max velocity

	uint32_t getMaxVelocity() const noexcept
	{
		return m_maxVelocity;
	}

	void setMaxVelocity(uint32_t maxVelocity = DEFAULT_MAX_VELOCITY) noexcept
	{
		m_maxVelocity = maxVelocity;
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

	enum { VERTEX, DALPHA, ALPHA, VELOCITY, 
		DEFAULT_DIRECTION = 0, 
		DEFAULT_ANGLE = 20, 
		DEFAULT_COUNT = 10000, 
		DEFAULT_SPRAY_AMOUNT = 50, 
		DEFAULT_SPAN = 50, 
		DEFAULT_GAP = 0, 
		DEFAULT_MAX_VELOCITY = 100 
	};


	// following parameters are used to customize the exhaust animation

	
	sf::Vector2f m_source;	// source point of spray

	double m_direction;	// direction of spray in angle

	uint16_t m_angle;	// spread of the spray in angle

	
	sf::Color m_color;	// color of the exhaust

	const uint32_t m_count;	// total count of particles in this simulation

	uint16_t m_sprayAmount;	// no. of particles created by one call to spray()

	uint16_t m_span;	// length of the tail of exhaust while it's not moving

	uint8_t m_gap;	//	distance between the source and the place where the particles appear

	uint32_t m_maxVelocity;	// maximum velocity of the particles
};