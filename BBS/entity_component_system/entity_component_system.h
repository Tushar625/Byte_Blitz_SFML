#pragma once

#include<vector>

#include<tuple>

#include<type_traits>


namespace bb
{
	template<typename ENTITY_BITMASK_TYPE, uint16_t RESERVE_EXTRA_ENTITIES, typename... component_types>

	class ENTITY_COMPONENT_SYSTEM;

	template<typename... component_types>

	struct ECS;
}



/*
	This is a simple ECS (Entity Component System) implementation.

	**********************************
	Let me describe it's inner working
	**********************************

	Component Arrays (Vectors):
	---------------------------

	Entities in this ECS are represented by IDs, which are simple indices. Each type
	of component is stored in a seperate array, implemented using std::vector, and an
	entity ID is used to access its corresponding component. These component arrays
	(vectors) are grouped together in a tuple.

	entity_id  ->  0  1  2 ... n

	component_a : [0][1][2]...[n]
	component_b : [0][1][2]...[n]
	component_c : [0][1][2]...[n]
	...
	component_z : [0][1][2]...[n]

	component tuple : (component_a, component_b, component_c, ..., component_z)

	Entity Bitmasks:
	----------------

	The ECS also maintains an array of bitmasks to track which components each entity
	possesses. Each bitmask corresponds to an entity, with each bit representing a
	specific component - 1 if the entity has it, 0 otherwise. The entity ID acts as the
	index to access these bitmasks, providing a clear mapping between entities and their
	components.

	entity_id    ->  0  1  2 ... n

	Bitmask Array : [0][1][2]...[n]

	example, bitmask of an entity, say we have 5 components,

	/^\                     part of this entity?
	 1   ->   component_a        Yes
	 0   ->   component_b          Neh
	 1   ->   component_c           Yes
	 1   ->   component_d          Yes
	 0   ->   component_e        Neh
	\+/

	Adding and Deleting Entity:
	---------------------------

	For managing entities, the system operates as a stack. New entities are added to the
	top of the stack. When an entity is deleted, it is replaced by the top entity, and
	the stack size is reduced by one. This keeps the components of the live entities packed
	in an contiguous array, without any data shifting and give better performance.

	example,

	{entity 0} <- {entity 1} <- {entity 2}

	<*> Add "entity 3"

	{entity 0} <- {entity 1} <- {entity 2} <- {entity 3}

	entity 3 added at the back

	<*> Delete "entity 0"

	{entity 3} <- {entity 1} <- {entity 2}

	entity 3 replaces entity 0 at the back

	*******************
	How to use this ECS
	*******************

	Explained with an example, say, we need to store 4 components,

	/----------\
	|Components|
	+----------+
	|Type  | Id|
	+----------+
	|int   | 0 |
	|float | 1 |
	|pos   | 2 |
	|int   | 3 |
	\----------/

	'pos' - a structure with 2 member data x and y
	
	Remember, Component vectors are stored in a tuple, Ids are used as indices to
	access a particular component vector from the tuple.

	You can use enum to add names to these component Ids,

	enum components{comp0, comp1, comp2, comp3};

	Creating ECS Object:
	--------------------

	ENTITY_COMPONENT_SYSTEM<uint8_t, 100, int, float, pos, int> ecs;

	uint8_t is the type of entity bitmask, no. of bits in entity bitmask = maximum no.
	of components.

	100 is the "RESERVE_EXTRA_ENTITIES", if create_entity() runs out of space, it extends
	internal vectors by "RESERVE_EXTRA_ENTITIES" to store new entities.

	Rest of the arguments specify component types.

	The order of the component types is important, as it determines the component ids.

	To reduce your effort, I provide a structure that only takes the component types, use
	it as follows,

	ECS<int, float, pos, int>::C8 ecs;

	C8 is a type defined in ECS structure that sets ENTITY_BITMASK_TYPE as uint8_t, similarly,
	to make it uint16_t, uint32_t, uint64_t use C16, C32, C64 respectively.

	By default RESERVE_EXTRA_ENTITIES is set to 100 inside ECS class.

	Adding an Entity:
	-----------------

	// adds all the components to the new entity

	auto entity = ecs.create_entity();

	// adds some of the components to the new entity

	auto entity = ecs.create_entity<comp0, comp2>();

	create_entity() has two overloads one takes no arguments other is a variadic template
	function, that takes ids of the components you want to add as template arguments.
	
	create_entity() returns an ENTITY object, ENTITY is a wrapper structure for an enity.
	More on ENTITY later.

	create_entity() can allocate extra storage for "RESERVE_EXTRA_ENTITIES" number of entities
	if required.

	Deleting an Entity:
	-------------------

	ecs.kill_entity(entity);

	Takes an ENTITY object, kills it be replacing it with the last entity... as mentioned
	above, in the inner working section.

	Accessing a Component Vector:
	-----------------------------

	std::vector<pos>& positions = ecs.component<comp2>();

	Takes "component id" as template argument.
	
	Returns a reference to the component vector, note that the type of the returned vector
	reference is "std::vector<component type>".

	Accessing an Entity:
	--------------------

	auto& entity = ecs.entity(2);

	Takes the entity id (here, 2), returns a reference to corresponding ENTITY object.
	
	I used "auto" as the name of the datatype, "ENTITY_COMPONENT_SYSTEM<int, float, pos, int>::ENTITY",
	is very long.

	Get the Number of Components and Entities:
	------------------------------------------

	ecs.component_count();

	ecs.entity_count();

	Reserve Extra Space to Store New Entities:
	------------------------------------------

	ecs.reserve_extra(100);

	Reserves space for 100 new entities, i.e., we can create 100 new entities without allocating any
	extra space. This is good if you want to add lots of new entities quickly.

	Deallocate all Dynamic Memory:
	------------------------------

	ecs.clear();

	Do We Have any Live Entities or not:
	------------------------------------

	ecs.empty();

	------------------
	~~~~ Caution: ~~~~
	------------------
	
	entity() doesn't check "Entity Id" for the sake of performance,

	But kill_entity() checks "Entity Id" and reserve_extra() varifies its input, because these 2 functions
	won't be called as frequently as others.

	create_entity(), component<>() check the "Component Ids", given as template arguments, in compile time.
	
*/

template<typename ENTITY_BITMASK_TYPE, uint16_t RESERVE_EXTRA_ENTITIES, typename... component_types>

class bb::ENTITY_COMPONENT_SYSTEM
{


	/*
		the bitmasks can be 8 to 64 bits long depending on the size of ENTITY_BITMASK_TYPE
		so, no. of components must be <= sizeof(ENTITY_BITMASK_TYPE)
	*/

	static_assert(

		(sizeof... (component_types) > sizeof(ENTITY_BITMASK_TYPE)),

		"!!!! Number of components must not exceed size of an entity bitmask !!!!"
		
	);



	static_assert(

		std::is_unsigned_v<ENTITY_BITMASK_TYPE>,

		"!!!! ENTITY_BITMASK_TYPE must be unsigned !!!!"
		
	);



	using c_type = std::tuple<std::vector<component_types>...>;	// tuple type to store the components



	public:



	/*
		This structure provides a wrapper for each entity, i.e., this structure can be used to represent
		an entity in this ECS

		The constructure takes reference to an object of this ECS class and the id of an entity, which is
		used to access the data corresponding to that entity

		********************************
		How to use this ENTITY structure
		********************************

		Explained with an example,

		Let, this is the ECS object,

		ECS<int, float, pos, int>::C8 ecs;
		
		As we can see, our ECS object has 4 components,

		/----------\
		|Components|
		+----------+
		|Type  | Id|
		+----------+
		|int   | 0 |
		|float | 1 |
		|pos   | 2 |
		|int   | 3 |
		\----------/

		'pos' - a structure with 2 member data x and y

		In ECS object Component vectors are stored in a tuple, Ids are used as indices to access a particular
		component vector from the tuple

		You can use enum to add names to these component Ids,

		enum components{comp0, comp1, comp2, comp3};

		Create an Entity Wrapper Object:
		--------------------------------
		
		ENTITY_COMPONENT_SYSTEM<int, float, pos, int>::ENTITY entity(ecs, 0);	// entity_id is 0

		ectity_id is optional, its default value is 0, you can update it later to access different entities
		like,
		
		entity.id = new_id;

		Note:
		
		Usually, I don't use this big structure name to create an Entity object, just use the entity()
		method of ECS object, as shown below,

		auto& entity = ecs.entity(0);

		[entity(entity_id) returns a reference to an ENTITY object, whose "id" is set to entity_id]

		Add Components:
		---------------

		entity.add<comp0, comp1, comp3>();

		Note that, this and the following methods, accepts the Component Ids as template arguments.

		Remove Components:
		------------------

		entity.remove<comp0, comp3>();

		Access a Component:
		-------------------

		entity.get<comp0>();

		returns a reference to the component with Id == comp0 of this entity

		Check If an Entity Has Given Components or Not:
		-----------------------------------------------

		entity.has<comp0, comp3>();

		Check If an Entity Object Has Valid Entity Id or Not:
		-----------------------------------------------------

		entity.valid();

		------------------
		~~~~ Caution: ~~~~
		------------------
		
		None of these functions check the "Entity Ids" (excluding valid()) for the sake of performance.

		But "Component Ids", given as template arguments, are checked in compile time.
	*/

	struct ENTITY
	{
		
		size_t id;	// entity id, an index if entity_list vector
		
		ENTITY_COMPONENT_SYSTEM &ecs;	// reference to ECS object to access ECS methods

		
		constexpr ENTITY(ENTITY_COMPONENT_SYSTEM &ecs_in, size_t id_in = 0) : ecs(ecs_in), id(id_in)
		{}

		
		/*
			add multiple components to this entity
		*/

		template<uint8_t... index_of_component> requires(
			(0 <= index_of_component && index_of_component < sizeof...(component_types)) && ...
		)

		constexpr void add() noexcept
		{
			ecs.entity_list[id] |= (((ENTITY_BITMASK_TYPE)1 << index_of_component) | ...);
		}

		
		/*
			remove multiple components from this entity
		*/

		template<uint8_t... index_of_component> requires(
			(0 <= index_of_component && index_of_component < sizeof...(component_types)) && ...
		)

		constexpr void remove() noexcept
		{
			ecs.entity_list[id] &= ~(((ENTITY_BITMASK_TYPE)1 << index_of_component) | ...);
		}

		
		/*
			get a component of this entity
		*/

		template<uint8_t index_of_component>

		constexpr std::tuple_element<index_of_component, c_type>::type::value_type& get() noexcept
		{
			return ecs.component<index_of_component>()[id];
		}

		
		/*
			does this entity has the given components or not
		*/

		template<uint8_t... index_of_component> requires(
			(0 <= index_of_component && index_of_component < sizeof...(component_types)) && ...
		)

		constexpr bool has() const noexcept
		{
			return (ecs.entity_list[id] & (((ENTITY_BITMASK_TYPE)1 << index_of_component) | ...));
		}

		
		/*
			checks if this entity is valid or not based on it's id
		*/

		constexpr bool valid() const noexcept
		{
			return 0 <= id && id <= ecs.top;
		}
	};



	private:


	
	std::vector<ENTITY_BITMASK_TYPE> entity_list;	// array of entity bitmasks

	c_type component_tuple;	// stores all the component vectors

	ENTITY temp_entity;	// temporary entity object used for internal opperations

	long long top;	// index of the last entity



	public:

	

	constexpr ENTITY_COMPONENT_SYSTEM() : temp_entity(*this), top(-1)
	{}



	// component functions



	/*
		This function returns a reference to the component vector corresponding to the specified "component id".
		This function takes the "component id" as a template argument.

		Component vectors are indexed based on the order of their types in the template arguments when
		the ECS object is declared. By convention, indexing starts at 0 and follows the sequence of
		types provided.

		Internally, components are stored as std::tuple. This function simply accesses an element of this tuple
		using "component id" as index and returns a reference to it.
	*/

	template<uint8_t index_of_component>

	constexpr std::tuple_element<index_of_component, c_type>::type& component()
	{
		return std::get<index_of_component>(component_tuple);
	}



	// entity functions



	/*
		adds a new entity to the ECS and returns it as an entity object
		
		lets you specify the ids of the components to be added to this entity as template arguments
	*/

	template<uint8_t... index_of_component> requires(
		(0 <= index_of_component && index_of_component < sizeof...(component_types)) && ...
	)

	constexpr ENTITY create_entity() noexcept
	{
		// check for overflow

		if (top >= entity_list.size() - 1)
		{
			// overflowing, increase capacity

			reserve_extra(RESERVE_EXTRA_ENTITIES);
		}

		temp_entity.id = ++top;

		entity_list[top] = (((ENTITY_BITMASK_TYPE)1 << index_of_component) | ...);

		return temp_entity;
	}


	/*
		overloaded version of previous function
		
		it adds all available components to the new entity
	*/

	constexpr ENTITY create_entity() noexcept
	{
		// check for overflow

		if (top >= entity_list.size() - 1)
		{
			// overflowing, increase capacity

			reserve_extra(RESERVE_EXTRA_ENTITIES);
		}

		// incrementing the top to push a new entity

		temp_entity.id = ++top;

		entity_list[top] = -1;
		
		return temp_entity;
	}


	/*
		exchanges the given entity with the last entity before killing the last entity
		this ensures a packed entity list, which provides better performance
	*/

	constexpr void kill_entity(ENTITY &entity) noexcept
	{
		// we don't need to check for top here this valid() function does that

		if(entity.valid())
		{
			// killing an entity only if it's valid

			// this entity will be replaced with the last entity

			entity_list[entity.id] = entity_list[top];

			// components of this entity will be replaced with the components of last entity

			std::apply([&](auto&&... args) {((args[entity.id] = args[top]), ...); }, component_tuple);

			// decrement top to pop out the last entity

			--top;
		}
	}


	/*
		extend the entity_list and all of component_lists to hold some more
		entities and their components
	*/

	constexpr void reserve_extra(size_t extra) noexcept
	{
		if ((entity_list.size() - entity_count()) >= extra)
		{
			// extra entities can be accomodated without allocating more space

			return;
		}

		// need to allocate some more space

		size_t required_capacity = entity_count() + extra;

		// allocating space for the entities
		
		entity_list.resize(required_capacity);

		// allocating space for their components

		std::apply([&](auto&&... args) {((args.resize(required_capacity)), ...); }, component_tuple);
	}


	/*
		access an entity as an entity object using the entity id

		!!!! Caution: the "Entity Ids" are not checked !!!!
	*/

	constexpr ENTITY& entity(size_t id) noexcept
	{
		temp_entity.id = id;

		return temp_entity;
	}


	constexpr size_t component_count() const noexcept
	{
		return sizeof... (component_types);
	}


	// no. of alive entities (the entities not deleted with kill_entity())

	constexpr size_t entity_count() const noexcept
	{
		return top + 1;
	}


	// clear all allocated memory

	constexpr void clear() noexcept
	{
		top = -1;

		entity_list.clear();

		std::apply([&](auto&&... args) {((args.clear()), ...); }, component_tuple);
	}


	// returns true if all entities are dead (deleted with kill_entity())

	constexpr bool empty() const noexcept
	{
		return top == -1;
	}
};



/*
	this is wrapper type to make it easier to use ENTITY_COMPONENT_SYSTEM<>
	it provides the first two template arguments

	ECS<int, double, float>::C8 ecs;

	        max component
	C8  =>    8
	C16 =>    16
	C32 =>    32
	C64 =>    64

	If you want to change "RESERVE_EXTRA_ENTITIES", unfortunately you have to
	use ENTITY_COMPONENT_SYSTEM<> template
*/

template<typename... component_types>

struct bb::ECS
{
	/*
		uint8_t is 8 bits long and allows for 8 components max, same goes for the others
	*/

	using C8 = ENTITY_COMPONENT_SYSTEM<uint8_t, 100, component_types...>;

	using C16 = ENTITY_COMPONENT_SYSTEM<uint16_t, 100, component_types...>;

	using C32 = ENTITY_COMPONENT_SYSTEM<uint32_t, 100, component_types...>;

	using C64 = ENTITY_COMPONENT_SYSTEM<uint64_t, 100, component_types...>;
};