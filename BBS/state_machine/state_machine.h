#pragma once

#include<limits>


namespace bb
{
	class BASE_STATE;

	class STATE_MACHINE;
}


/*
	how to use:

	Basic Usage:

	create an object of STATE_MACHINE class. Its constructor takes nothing.

	Example: STATE_MACHINE sm;

	now create the states, each state is a class inherited from BASE_STATE class ("public inheritance")
	with proper definition of Enter(), Update(), Render(), Exit() functions, and create objects of those
	'state classes'. These objects will serve as the states.

	Example:

	class initial_state : public BASE_STATE
	{
		void Enter()	// initialze this state
		{...}

		int Update(double dt)	// update this state
		{...}

		void Render()	// render this state
		{...}

		void Exit()		// destroy this state
		{...}
	} initial;

	inside the Update() you call "sm.change_to()" to change the current state of the state machine
	this function takes a reference to a state object and makes it the current state. this function
	is also used to set the initial state of the state machine.

	note: a call to "sm.change_to()" from Update() of a state must be last action of this function,
	as sm.change_to() calls the Exit() of current state i.e., destroys the current state

	Example:

	extern class play_state play;	// declaring the state object which is defined later

	//==========================
	// ~~~~~ initial state ~~~~~
	//==========================

	class initial_state : public BASE_STATE
	{
		...

		int Update(double dt)
		{
			...
			sm.change_to(play);	// change state: 'initial' -> 'play' 
			...
		}

		...
	};
	
	initial_state initial;	// creating the state object

	//=======================
	// ~~~~~ play state ~~~~~
	//=======================
	
	class play_state : public BASE_STATE
	{
		...
	};
	
	play_state play;	// creating the state object
	
	// the function that initializes the game

	Game::Create()
	{
		...
		sm.change_to(initial);	// setting 'intial' as initial state
		...
	}

	call the update and render methods of STATE_MACHINE from update and render
	functions of the game loop, they inturn call the update and render methods
	of current state, respectively.

	Update() of the states can also return 'int' data, which is again returned
	by the Update() of state machine so, you can use this to signal the Update()
	of the game loop to stop the game; if no current state is set, Update() of
	state machine returns the minimum int value, a garbage value.

	Example:

	class initial_state : public BASE_STATE
	{
		...

		int Update(double dt)
		{
			...
			if(...)
			{
				return 0;	// using '0' to signal exit
			}
			...
			return -1;
		}

		...
	};

	...

	bool Game::Update(double dt)
	{
		...
		if(sm.Update(dt) == 0)
		{
			// code to stop the game loop
			...
		}
		...
	}

	==================================================================================

	Data of game states:

	You can simply have data members in state class, but if you want to have some data
	that will only exist while the state is in use, you should do it the following way,

	>>> Keep a structure in the class that represents or contains the data
	
	>>> Have a pointer to that structure as member data

	>>> Dynamically allocate the structure in Enter()

	>>> Destroy the structure in Exit()

	Example:

	class yoo_state : public BASE_STATE
	{
		
		// a structure nicely encaptulate the data used in this state locally


		struct data
		{
			// declare the variables i.e., data for the state, here

			...

			// initialize the variables

			data(...)
			{...}

		} *p_data;


		yoo_state() : p_data(NULL)
		{...}


		void Enter()
		{
			...
			// creating the local state data

			p_data = new data(...);
			...
		}


		void Exit()
		{
			...
			// destroying the local state data

			delete b_data;
			...
		}

	};

	==================================================================================

	Transferring data between states:

	This state machine does not provide any facility to send data while changing state
	but there's a way to do so. Simply keep some public data members and use then to
	transfer data while changing states, see the example below. Notice that, to send
	data to an state object declated later in the code we utilise a function, this
	processe is described nicely in the example below.

	Example:

	extern class play_state play;	// declaring the state object which is defined later

	void set_play_state(int play_data);	// declaring the function used to send data to play state object

	//==========================
	// ~~~~~ initial state ~~~~~
	//==========================

	class initial_state : public BASE_STATE
	{
		...

		public:

		int initial_data;

		private:

		...

		int Update(double dt)
		{
			...
			set_play_state(some_data);	// sending data to play state

			sm.change_to(play);	// change state: 'initial' -> 'play' 
			...
		}

		...
	};
	
	initial_state initial;	// creating the state object

	//=======================
	// ~~~~~ play state ~~~~~
	//=======================
	
	class play_state : public BASE_STATE
	{
		...
		
		public:

		int play_data;

		private:

		...
	};
	
	play_state play;	// creating the state object

	// since we can't access members of 'play' before declaring it
	// we define this function here, which is again declared before
	// initial_state definition and call it to send data to play state

	void set_play_state(int play_data)
	{
		play.play_data = play_data;
	}
	
	// the function that initializes the game

	Game::Create()
	{
		...
		initial.initial_data = some_data;	// sending data to play state
		
		sm.change_to(initial);	// setting 'intial' as initial state
		...
	}

*/


/*
	this is a template for game states,

	game states are actually classes that inherite this class and redefine the
	virtual methods as per their requirements
*/

class bb::BASE_STATE
{
	public:

	virtual void Enter()
	{}

	virtual int Update(double dt)
	{
		return std::numeric_limits<int>::min();
	}

	virtual void Render()
	{}

	virtual void Exit()
	{}
};


/*
	this is the actual state machine, it has a BASE_STATE pointer that points the
	current state
*/

class bb::STATE_MACHINE
{
	/*
		points to the current states or points to NULL if no current state given
	*/
	
	BASE_STATE *current_state;


	public:

	STATE_MACHINE() : current_state(nullptr) // no current state is set initially
	{}


	/*
		state transition
		exit from current state and set the next sate as its current state
	*/

	template<typename STATE_TYPE>
	
	void change_to(STATE_TYPE& next_state)
	{
		if(current_state)
			current_state->Exit();  // exit from current state

		current_state = &next_state;	// store address of this state

		current_state->Enter(); // enter new state
	}


	/*
		gets elapsed time from game loop and supplies it to current state
		returns an int to the Update() of game loop, update() of the game
		states can use this to send some signal (eg, stop game loop) to
		the Update() of game loop
	*/

	int Update(double dt)
	{
		if(current_state)
		{
			return current_state->Update(dt);
		}

		return std::numeric_limits<int>::min();
	}


	/*
		rander the state
	*/

	void Render()
	{
		if(current_state)
			current_state->Render();
	}
};