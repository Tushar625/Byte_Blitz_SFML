#pragma once

#include<initializer_list>


/*
	how to use:

	each game state is a class inherited ("public inheritance") from BASE_STATE class, with redefinition
	of necessary functions.

	create objects of those state classes (global or local).

	now create an object of STATE_MACHINE class (global or local). Its constructor
	takes a list of pointers game states as argument. These are pointers to those
	state objects created in previous step.

	Example: STATE_MACHINE sm({&state0, &state1, &state2, &state3});

	[note, type of each state is different but they are inherited from the same BASE_STATE class so, a BASE_STATE
	type pointer can hold their address]

	if needed, use set member function to put address of proper state in proper index (its
	claver to use enum to represent those indices).

	call the update and render methods of STATE_MACHINE from update and render
	functions of the game loop.

	by default the state machine constructor sets the first state as the initial state to change it
	the constructor takes a default parameter, the index of initial state (set to 0 by default).

	and its done.

	adv: entire description of a state gets enclosed in the state classes
*/


/*
	this ia basically a template for game states,

	basically game states are classes that inherites this
	class and redefine the virtual methods as per their requirement

	watch how we use Update method to change the game state
*/

class BASE_STATE
{
	public:

	virtual void Enter()
	{}

	/*
		returns the index of the next state if state change is needed
		else return -1

		i.e., according to the index of the state returned by it Update
		method of STATE_MACHINE changes the game state
	*/

	virtual int Update(double dt)
	{
		return -1;
	}

	/*
		we don't sent the fps string to the rander as it would increase
		the typing effort as there will be many game states
	*/

	virtual void Render()
	{}

	virtual void Exit()
	{}
};


/*
	this is the actual state machine, it mentains an array
	that points to all the game states
*/

class STATE_MACHINE
{
	/*
		points to the current states or points to NULL if no current state given
	*/
	
	BASE_STATE *current_state;


	/*
		its an array of pointers where each element points to a state
	*/

	BASE_STATE **state_list;


	int n;  // no. of game states


	public:


	/*
		takes a list of pointers to state objects
	*/

	STATE_MACHINE(std::initializer_list<BASE_STATE *> state_list, int initial_state = 0) : current_state(nullptr) // no current state is set initially
	{
		n = (int)state_list.size();	// getiing the no. of the states

		this->state_list = new BASE_STATE*[n]{0};    // allocating memory for list of states

		int i = 0;	// index for the state_list

		// by now the state_list is created, load all the states into it

		for(auto state: state_list)
		{
			this->state_list[i++] = state;
		}

		change_to(initial_state);	// start the first state by default
	}


	// destructor deallocates the state list

	~STATE_MACHINE()
	{
		delete[] state_list;
	}


	// set inidividual states by their index (doesn't have much use these days)

	void set(int i, BASE_STATE *p_state)
	{
		if(i >= 0 && i < n)
			state_list[i] = p_state;
	}


	/*
		state transition
		exit from current state and set the current sate as ith state
	*/

	bool change_to(int i)
	{
		if(i < 0 || i >= n) // i is not accurate
			return false;

		if(current_state)
			current_state->Exit();  // exit from current state

		current_state = state_list[i];

		if(current_state)
			current_state->Enter(); // enter new state

		return true;
	}


	/*
		gets elapsed time from game loop and supplies it to current state
		returns the new state set by update function of current state
		-1 if no current state found
	*/

	int Update(double dt)
	{
		int i = -1;

		if(current_state)
		{
			i = current_state->Update(dt);

			// if the index of the state returned is valid call the state changer

			if(i >= 0 && i < n)
				change_to(i);
		}

		return i;
	}


	/*
		rander the state on a frame f1
	*/

	void Render()
	{
		if(current_state)
			current_state->Render();
	}
};