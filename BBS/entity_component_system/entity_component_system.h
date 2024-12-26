#pragma once

#include<concepts>

#include<bitset>

#include<vector>

#include<initializer_list>

// component pool class, each type represents a component

template<typename... component_types>

class ENTITY_COMPONENT_SYSTEM
{
	struct ENTITY
	{
		size_t id;

		ENTITY_COMPONENT_SYSTEM &ecs;

		ENTITY(ENTITY_COMPONENT_SYSTEM &ecs_in, size_t id_in = 0) : ecs(ecs_in), id(id_in)
		{}

		/*
			add multiple components to this entity
		*/

		void add(size_t index_of_compoent) noexcept
		{
			ecs.entity_list[id].set(index_of_compoent);
		}

		/*
			remove multiple components from this entity
		*/

		void remove(size_t index_of_compoent) noexcept
		{
			ecs.entity_list[id].reset(index_of_compoent);
		}

		/*
			get a component of this entity
		*/

		template<typename type> requires (std::same_as<component_types, type> || ...)

		type& get(size_t index_of_compoent) noexcept
		{
			return ecs.component<type>(index_of_compoent)[id];
		}

		/*
			does this entity have the given components or not
		*/

		bool has(size_t index_of_compoent) const noexcept
		{
			return (is_alive() && ecs.entity_list[id].test(index_of_compoent));
		}

		/*
			is this entity alive
		*/

		bool is_alive() const noexcept
		{
			return ecs.entity_list[id].test(LIFE_FLAG);
		}

		/*
			checks if this entity is valid or not based on it's id
		*/

		bool is_valid() const noexcept
		{
			return 0 <= id && id < ecs.entity_count();
		}
	};

	/*
		FLAGS represent positions of some data flags in the bitmask of each entity

		if there are 'n' no. of components, first 'n' bits will represent the
		components, after that I keep LIFE_FLAG to indicate if the entity is in use
		or dead
	*/

	enum FLAGS{LIFE_FLAG = sizeof... (component_types), MASK_SIZE};

	// array of component vectors

	std::vector<uint8_t> component_list[sizeof... (component_types)];

	// array of entity bitmasks

	std::vector<std::bitset<MASK_SIZE>> entity_list;

	// array of dead entities

	std::vector<size_t> dead_entity_list;

	ENTITY temp_entity;

	public:

	// constructor creates individual components and component type to component index maps

	ENTITY_COMPONENT_SYSTEM() : temp_entity(*this)
	{}

	// component functions

	// type -> component vector map

	template<typename type> requires (std::same_as<component_types, type> || ...)

	std::vector<type>& component(size_t index_of_compoent) noexcept
	{
		return *(std::vector<type> *)(&component_list[index_of_compoent]);
	}

	// entity functions

	/*
		add a new entity to the ENTITY_COMPONENT_SYSTEM also specify its components
	*/

	ENTITY create_entity(std::initializer_list<size_t> index_of_compoent = {}) noexcept
	{
		if(dead_entity_list.empty())
		{
			// no entity to renew so adding new entity

			temp_entity.id = entity_list.size();

			entity_list.push_back(0);

			entity_list[temp_entity.id].set(LIFE_FLAG);

			// need to extend the component vectors

			size_t index = 0;

			(component<component_types>(index++).resize(entity_list.size()), ...);
		}
		else
		{
			// renew a dead entity and remove it from dead list

			temp_entity.id = dead_entity_list.back();

			dead_entity_list.pop_back();

			entity_list[temp_entity.id].reset();

			entity_list[temp_entity.id].set(LIFE_FLAG);
		}

		// adding components to this new entity

		if(index_of_compoent.size())
		{
			for (auto& index : index_of_compoent)
			{
				temp_entity.add(index);
			}
		}
		else
		{
			for (size_t index = 0; index < sizeof... (component_types); index++)
			{
				temp_entity.add(index);
			}
		}

		return temp_entity;
	}

	/*
		kill the given entity and add it to the dead entity list
	*/

	void kill_entity(const ENTITY &entity) noexcept
	{
		if(entity.is_valid() && entity.is_alive())
		{
			// killing an entity only if it's valid and alive 

			entity_list[entity.id].reset();

			// push the index of the killed entity into dead entity list

			dead_entity_list.push_back(entity.id);
		}
	}

	/*
		exchange the given entity with the last alive entity before killing it
		to ensure packed entity list
	*/

	void kill_entity_packed(ENTITY &entity) noexcept
	{
		ENTITY temp(*this);	// to store the last alive entity

		if(entity.is_valid() && entity.is_alive())
		{
			// killing an entity only if it's valid and alive

			// this entity will be replaced with the last alive entity

			// finding the last alive entity

			/*for (long long i = entity_list.size() - 1; i >= 0; i--)
			{
				temp.id = i;

				if(temp.is_alive())
				{
					// last alive entity is found

					break;
				}
			}*/

			temp.id = live_entity_count() - 1;

			// replace the killed entity with last alive entity

			entity_list[entity.id] = entity_list[temp.id];

			size_t index = 0;

			((entity.get<component_types>(index++) = temp.get<component_types>(index)), ...);

			// kill the last alive entity

			entity_list[temp.id].reset();

			// push the index of the killed entity into dead entity list

			dead_entity_list.push_back(temp.id);
		}
	}

	/*
		extend the entity holding capacity of this ecs
	*/

	void reserve_extra(size_t extra) noexcept
	{
		if(extra <= dead_entity_list.size())
		{
			/*
				if the extra capacity required is less than the dead entity
				count we don't need to reserve new space
			*/
			
			return;
		}

		/*
			calculating how many extra spaces we need to acqire (extra -
			dead_entity_list.size()) and adding it to the current size of
			entity list to get the total size we need to reserve
		*/

		size_t required_capacity = entity_list.size() + extra - dead_entity_list.size();
		
		entity_list.reserve(required_capacity);

		size_t index = 0;
		
		(component<component_types>(index++).reserve(required_capacity), ...);
	}

	ENTITY& entity(size_t id) noexcept
	{
		temp_entity.id = id;

		return temp_entity;
	}

	size_t component_count() const noexcept
	{
		return sizeof... (component_types);
	}

	size_t entity_count() const noexcept
	{
		return entity_list.size();
	}

	size_t live_entity_count() const noexcept
	{
		return entity_list.size() - dead_entity_list.size();
	}

	/*
		clear every thing
	*/

	void clear() noexcept
	{
		dead_entity_list.clear();

		entity_list.clear();

		for(auto& component : component_list)
		{
			component.clear();
		}
	}

	bool empty() const noexcept
	{
		return entity_list.empty();
	}
};