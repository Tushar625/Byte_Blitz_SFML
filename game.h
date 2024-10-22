
#pragma once


#include"game_loop/game_loop.h"	// general purpose game loop

#include"state_machine/state_machine.h"	// general purpose game state machine

#include"button/button.h" // general purpose button handler

#include"system/window.h"

#include"system/input.h"


int arg_count;	// to accept no. of command line arguments

char **arg;	// to accept command line arguments

int return_value = 0;	// value to be returned when the program ends


/*
	To run a game you need to run a game loop, in c++ usually one has to inherit
	a class in which the game loop is defined and override the create(), update(),
	render() like functions which are called by the game loop, create an object of
	that derived class and call a function to start the game loop to start the game.

	But here I have designed a new, better way to write and start a game.

	Now, this new feature lets you define only the necessary functions of game loop before
	you run the game, the game loop will start autometically, default FPS will be 20, though
	it can be changed by set_fps() function of GAME_LOOP class.

	How it works:

	In this file a new class 'Game' has been created which inherits GAME_LOOP class of
	game_loop.h.
	
	It contains declerations for all the functions of the game loop, which can be
	enabled by macros (Enable only the functions which you want to redefine). Some
	functions are defined here to provided a default definition of them, macros
	corresponding to these functions can be used to disable the definition, so that
	user can redefine them.

	Note that Create() function must be defined there is no way to disable it, as it
	must be overridden. (Reason for this design choice is that Create() is considerred
	to be the entry point for the game and it's a pure virtual function in GAME_LOOP
	class)
	
	If enabled a function must be redefined and the new definition will override its
	default or old definition.

	An object of this class "MY_GAME" is created, that represents the core of the game
	or game loop.
	
	Inside the main() function, defined at the very end of this file, MY_GAME.start_game() is called
	so that the game loop starts after all the required objects in the game source code has been
	created.

	How to use:

	Before including game.h, define the macros corresponding to the member function to
	set them up, these macros will allow user to either redefine them or use their default
	definition. (More about these macros later)

	Now, you can define the necessary member functions, Game::function_name(), the way
	we define a member function outside the class in c++.

	Remember that the decleration of Create() can't be supressed i.e., you must define
	Game::Create() function in the source code

	After you complete defining those functions just run the source code, the game loop
	will start autometically.

	By default the game loop starts at 30fps and 120ups. To set fps or ups or get fps use set_fps(),
	set_ups() or get_fps() functions of MY_GAME object From any where in the game source code.

	!!!! never call MY_GAME.start_game() from the game source code

	To sum it up, from top to bottom of the source code where you include this header file,
	
		you first define the necessary mecros

		then you include the game.h header file and other header files as needed for your game

		then you define any extra classes or global variables needed for the game

		after that, define member functions of 'Game' class

		and at the end run your game
*/

/*
	How to use the macros:

	According to my experience with this game engine, most of the times "Create(), Update() and Render()"
	functions are needed to be redefined.
	
	For the rest the default definitions are good for most of the times.

	Create() function is a pure virtual function of parent class so, it must be defined; Update() and
	Render() are declared by default i.e., one must define them, to use their default definition one must
	define "NO_GAME_UPDATE" and "NO_GAME_RENDER" macros respectively.

	Input(), Render_Thread_Init(), Clear(), Print() and Destroy() are usually not required to be redefined
	but if one wants to redefine them, he or she must define "GAME_INPUT", "GAME_RENDER_THREAD_INIT",
	"GAME_CLEAR", "GAME_PRINT", "GAME_DESTROY" macros respectively to disable or remove their default definition.

	**** This file only contains default definitions of Input(), Render_Thread_Init(), Clear(), Print() member
	**** functions, default definitions of other member functions are provided in parent class

	Summary:

	If you want to redefine only "Create(), Update() and Render()", you don't need to define any macro
*/

class Game : public GAME_LOOP
{
	bool Create() override;


	void Input() override;
	

	#ifndef NO_GAME_UPDATE
	
		bool Update(double dt) override;
	
	#endif


	void Render_Thread_Init() override;


    void Clear() override;


	#ifndef NO_GAME_RENDER

		void Render() override;
	
	#endif


    void Print() override;


	#ifdef GAME_DESTROY
	
		bool Destroy() override;
	
	#endif


	public:

	Game()
	{
		/*
			deactivate the window's context in main thread before
			starting the render thread
		*/

		WINDOW.setActive(false);
	}

	~Game()
	{
		WINDOW.close();
	}
} MY_GAME;	// object representing the core of this this game


/*
	default definitions of Input(), Clear(), Print() member functions
*/


#ifndef GAME_INPUT

	inline void Game::Input()
	{
		INPUT.scan();
	}

#endif


#ifndef GAME_RENDER_THREAD_INIT

	inline void Game::Render_Thread_Init()
	{
		// activate the window's context in render thread

		WINDOW.setActive(true);
	}

#endif


#ifndef GAME_CLEAR

	inline void Game::Clear()
	{
		WINDOW.clear(sf::Color(0, 0, 0));
	}

#endif


#ifndef GAME_PRINT

	inline void Game::Print()
	{
		WINDOW.display();
	}

#endif


/*
	the code inside main() is executed after all the objects defined in the game source
	code is created.

	Hence, we call start_game() inside main() so that the game loop starts to run only
	after all the necessary objects are created.

	!!!! after the game loop starts to run never call this start_game() function again
*/

#ifndef USE_MAIN

#define USE_MAIN main	// redefine this macro to use different main function say, "WinMain"

#endif

int USE_MAIN(int arg_count, char *arg[])
{
	::arg_count = arg_count;

	::arg = arg;

	MY_GAME.start_game();

	return return_value;
}
