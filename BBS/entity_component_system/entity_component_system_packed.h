#pragma once

#include<concepts>

#include<bitset>

#include<vector>

#include<initializer_list>

// component pool class, each type represents a component

template<typename... component_types>

class ENTITY_COMPONENT_SYSTEM_PACKED
{
	struct ENTITY
	{
		size_t id;

		ENTITY_COMPONENT_SYSTEM_PACKED &ecs;

		ENTITY(ENTITY_COMPONENT_SYSTEM_PACKED &ecs_in, size_t id_in = 0) : ecs(ecs_in), id(id_in)
		{}

		/*
			add multiple components to this entity
		*/

		constexpr void add(size_t index_of_compoent) noexcept
		{
			ecs.entity_list[id].set(index_of_compoent);
		}

		/*
			remove multiple components from this entity
		*/

		constexpr void remove(size_t index_of_compoent) noexcept
		{
			ecs.entity_list[id].reset(index_of_compoent);
		}

		/*
			get a component of this entity
		*/

		template<typename type> requires (std::same_as<component_types, type> || ...)

		constexpr type& get(size_t index_of_compoent) noexcept
		{
			return ecs.component<type>(index_of_compoent)[id];
		}

		/*
			does this entity have the given components or not
		*/

		constexpr bool has(size_t index_of_compoent) const noexcept
		{
			return ecs.entity_list[id].test(index_of_compoent);
		}

		/*
			checks if this entity is valid or not based on it's id
		*/

		constexpr bool is_valid() const noexcept
		{
			return 0 <= id && id < ecs.entity_count();
		}
	};

	// array of component vectors

	std::vector<uint8_t> component_list[sizeof... (component_types)];

	// array of entity bitmasks

	std::vector<std::bitset<sizeof... (component_types)>> entity_list;

	ENTITY temp_entity;

	long long top;

	public:

	// constructor creates individual components and component type to component index maps

	ENTITY_COMPONENT_SYSTEM_PACKED() : temp_entity(*this), top(-1)
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

	constexpr ENTITY create_entity(std::initializer_list<size_t> index_of_compoent = {}) noexcept
	{
		// add a new entity mask

		temp_entity.id = entity_list.size();

		entity_list.push_back(0);

		// need to extend the component vectors

		size_t index = 0;

		(component<component_types>(index++).resize(entity_list.size()), ...);
		
		// adding components to this new entity

		if(index_of_compoent.size())
		{
			for (auto& index : index_of_compoent)
			{
				entity_list[temp_entity.id].set(index);
			}
		}
		else
		{
			entity_list[temp_entity.id].set();
		}

		return temp_entity;
	}

	/*
		exchange the given entity with the last entity before killing it
		to ensure packed entity list
	*/

	constexpr void kill_entity(ENTITY &entity) noexcept
	{
		if(entity.is_valid())
		{
			// killing an entity only if it's valid

			// this entity will be replaced with the last entity

			auto endindex = entity_list.size() - 1;

			entity_list[entity.id] = entity_list[endindex];

			size_t index = 0;

			((component<component_types>(index++)[entity.id] = component<component_types>(index)[endindex]), ...);

			// kill the last entity and it's components

			entity_list.pop_back();

			index = 0;

			(component<component_types>(index++).pop_back(), ...);
		}
	}

	/*
		extend the entity holding capacity of this ecs
	*/

	void reserve_extra(size_t extra) noexcept
	{
		size_t required_capacity = entity_list.size() + extra;
		
		entity_list.reserve(required_capacity);

		size_t index = 0;
		
		(component<component_types>(index++).reserve(required_capacity), ...);
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
		return entity_list.size();
	}

	/*
		clear every thing
	*/

	constexpr void clear() noexcept
	{
		entity_list.clear();

		size_t index = 0;
		
		(component<component_types>(index++).clear(), ...);
	}

	constexpr bool empty() const noexcept
	{
		return entity_list.empty();
	}
};