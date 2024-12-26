#pragma once

#include<vector>

// component pool class, each type represents a component

template<typename... component_types>

class ENTITY_COMPONENT_SYSTEM
{
public:

	struct ENTITY
	{
		size_t id;

		ENTITY_COMPONENT_SYSTEM &ecs;

		ENTITY(ENTITY_COMPONENT_SYSTEM &ecs_in, size_t id_in = 0) : ecs(ecs_in), id(id_in)
		{}

		/*
			add multiple components to this entity
		*/

		template<typename... types> requires (std::convertible_to<types, size_t> || ...)

		constexpr void add(types... index_of_component) noexcept
		{
			ecs.entity_list[id] |= ((1 << index_of_component) | ...);
		}

		/*
			remove multiple components from this entity
		*/

		template<typename... types> requires (std::convertible_to<types, size_t> || ...)

		constexpr void remove(types... index_of_component) noexcept
		{
			ecs.entity_list[id] &= ~((1 << index_of_component) | ...);
		}

		/*
			get a component of this entity
		*/

		template<typename type> requires (std::same_as<component_types, type> || ...)

		constexpr type& get(size_t index_of_component) noexcept
		{
			return ecs.component<type>(index_of_component)[id];
		}

		/*
			does this entity have the given components or not
		*/

		template<typename... types> requires (std::convertible_to<types, size_t> || ...)

		constexpr bool has(types... index_of_component) const noexcept
		{
			return (ecs.entity_list[id] & ((1 << index_of_component) | ...));
		}

		/*
			checks if this entity is valid or not based on it's id
		*/

		constexpr bool is_valid() const noexcept
		{
			return 0 <= id && id <= ecs.top;
		}
	};

private:

	// array of component vectors

	std::vector<uint8_t> component_list[sizeof... (component_types)];

	// array of entity bitmasks

	std::vector<size_t> entity_list;

	ENTITY temp_entity;

	long long top;

	public:

	// constructor creates individual components and component type to component index maps

	ENTITY_COMPONENT_SYSTEM() : temp_entity(*this), top(-1)
	{}

	// component functions

	// type -> component vector map

	template<typename type> requires (std::same_as<component_types, type> || ...)

	constexpr std::vector<type>& component(size_t index_of_compoent) noexcept
	{
		return *(std::vector<type> *)(&component_list[index_of_compoent]);
	}

	// entity functions

	/*
		add a new entity to the ENTITY_COMPONENT_SYSTEM also specify its components
	*/

	template<typename... types> requires (std::convertible_to<types, size_t> || ...)

	ENTITY create_entity(types... index_of_component) noexcept
	{
		// check for overflow

		if (top >= entity_list.size() - 1)
		{
			// overflowing, increase capacity

			reserve_extra(8);
		}

		temp_entity.id = ++top;

		entity_list[top] = ((1 << index_of_component) | ...);

		return temp_entity;
	}

	ENTITY create_entity() noexcept
	{
		// check for overflow

		if (top >= entity_list.size() - 1)
		{
			// overflowing, increase capacity

			reserve_extra(8);
		}

		temp_entity.id = ++top;

		entity_list[top] = 0XFFFFFFFFFFFFFFFF;
		
		return temp_entity;
	}

	/*
		exchange the given entity with the last entity before killing it
		to ensure packed entity list
	*/

	void kill_entity(ENTITY &entity) noexcept
	{
		if(entity.is_valid())
		{
			// killing an entity only if it's valid

			// this entity will be replaced with the last entity

			entity_list[entity.id] = entity_list[top];

			size_t index = 0;

			((component<component_types>(index++)[entity.id] = component<component_types>(index)[top]), ...);

			// decrement top to pop out the last entity

			--top;
		}
	}

	/*
		extend the entity holding capacity of this ecs
	*/

	void reserve_extra(size_t extra) noexcept
	{
		if ((entity_list.size() - entity_count()) >= extra)
		{
			// extra entities can be accomodated without allocating more space

			return;
		}

		// need to allocate some more space

		size_t required_capacity = entity_count() + extra;
		
		entity_list.resize(required_capacity);

		size_t index = 0;
		
		(component<component_types>(index++).resize(required_capacity), ...);
	}

	constexpr ENTITY& entity(size_t id) noexcept
	{
		temp_entity.id = id;

		return temp_entity;
	}

	constexpr size_t component_count() const noexcept
	{
		return sizeof... (component_types);
	}

	constexpr size_t entity_count() const noexcept
	{
		return top + 1;
	}

	/*
		clear every thing
	*/

	constexpr void clear() noexcept
	{
		top = -1;

		entity_list.clear();

		size_t index = 0;
		
		(component<component_types>(index++).clear(), ...);
	}

	constexpr bool empty() const noexcept
	{
		return top == -1;
	}
};