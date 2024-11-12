#pragma once



#include<thread>

#include<atomic>

#include"../utility/fps_control.h"	// to coltrol the fps properly as it changes continuously making the graphics bad



#define SUCCESS 0

#define STOP_GAME_LOOP 1


namespace bb
{
	class GAME_LOOP;
}


/*
	How it works:-

	This is a very simple game loop with seperate update and render loop that
	run parallelly at fixed FPS and UPS, and calls some functions repeatedly
	to run the game, these functions will be defined by the user according
	their requirements

	Describing this functions

	=> bool Create()

	This is to initialize the game before starting the game loop, this
	should be used to provide a safe entry point for the game loop not
	to load any game assets like sprites or sounds etc.

	It should return 'SUCCESS' if the game is initialised successfully
	else return '!SUCCESS'

    >>>> Two subsequent functions makes the input and processing system

        => void Input()

        It contains definition of the input system i.e., basically the
        functions or macros used for the I/P system

        => bool Update(double passed_time)

        It contains the game logic processing system, it takes an argument
        called passed_time that gives it the time passed between two calls
        i.e., time passed in the last iteration in seconds

        It can also stop the game loop by returning 'STOP_GAME_LOOP' but if
        you don't want to stop the game loop return '!STOP_GAME_LOOP'

    >>>> Three subsequent functions makes the graphics rendering system

		=> void Render_Thread_Init()

		Initializes the Rendering thread. it is called only once when the
		rendering thread is started

        => void Clear()

        To clear the default frame or canvas.
        
        => void Render()

        To rander or draw on the default frame or canvas, it receives the fps as a string.

        => void Print()

        To print the default frame or canvas on screen.

	=> bool Destroy()

	This is to Destroy the game before exiting the game_loop() function
	It also deallocate any memory dynamically allocated by Create()

	It should return 'SUCCESS' if the game is destroyed successfully
	else return '!SUCCESS', in this case we start the game loop again
*/


class bb::GAME_LOOP
{
	FPS_CONTROL FRAME_RATE_STABILIZER;

	std::atomic_bool loop_continue, lock;

	double udt, ut_accumulator;
	
	std::atomic<double> fps;

	/*
		main game loop systems
	*/

	/*
		render loop
	*/

	void render_loop()
	{
		Render_Thread_Init();

		FRAME_RATE_STABILIZER.initialize();	// run with stabilized fps

		do{
			// >>>> graphics rendering system

			{
				// waiting for render to be unlocked

				while(lock == false);

				fps = 1 / FRAME_RATE_STABILIZER.dt;

				Clear();  // to clear the default frame or canvas

				Render();    // to rander or draw on the default frame or canvas

				ut_accumulator += FRAME_RATE_STABILIZER.dt;

				lock = false;	// unlock the update
			}

			Print();  // to print the default frame or canvas on screen

			FRAME_RATE_STABILIZER.put_delay();	// dynamic delay to stabilize the fps

		}while(loop_continue);
	}

	void game_loop()
	{
		fps = 0;

		ut_accumulator = 0;	// keeps time for update

		lock = true;	// true => render, false => update

		/*
			to initialize game i.e. to create the initial state of
			the game before entering the game loop
		*/

		if(Create() != SUCCESS)
		{
			Destroy();	// if we fail to initialize the game then we must destroy it before returning

			return;
		}

		while(true)
		{
			loop_continue = true;

			// start render loop

			std::thread render_thread(&GAME_LOOP::render_loop, this);

			// start update loop

			do{
                // >>>> input and processing system
                
				{
					// waiting for update to be unlocked

					while(lock == true);

					while(ut_accumulator > udt)
					{
						Input();

						if(Update(udt) == STOP_GAME_LOOP)
						{
							loop_continue = false;	// stop the render loop which later stops update loop
						}

						ut_accumulator -= udt;
					}

					lock = true;	// unlock the render
				}

			}while(loop_continue);

			// wait for render thread to end

			if(render_thread.joinable())
			{
				render_thread.join();
			}

			if(Destroy() == SUCCESS)
				break;
		}
	}

	/*
		this function is made pure virtual so that

		we can't create an object of this class and

		start the game loop without defining it in our game
	*/

	virtual bool Create()=0;

	virtual void Input()
	{}

	virtual bool Update(double dt)
	{
		return !STOP_GAME_LOOP;
	}

	// a simple initializer for render thread

	virtual void Render_Thread_Init()
	{}

    virtual void Clear()
	{}

	virtual void Render()
	{}

    virtual void Print()
	{}

	virtual bool Destroy()
	{
		return SUCCESS;
	}

	public:

		double get_fps()
    	{
       		return fps;
		}

		void set_fps(double target_fps = 30)
    	{
       		FRAME_RATE_STABILIZER.set(target_fps);
		}

		void set_ups(double target_ups = 120)
    	{
			udt = 1 / (target_ups);
		}

		// call it from the constructor of the game object to start the game

		void start_game(double target_fps = 30, double target_ups = 120)	// start_game also sets the fps, default fps is 20.0
		{
			set_fps(target_fps);

			set_ups(target_ups);

			game_loop();
		}
};



/*
	How to use:-

	In the source file of your game first include this header file "game_loop.h"
	and all the other header files you may need.

	after that,

	Create a class for your game which will inherit the class "GAME_LOOP" (you
	can use any of public, private, protected inheritence).

	This class is your main game class, in it you need to override the virtual
	functions of GAME_LOOP class, Create() function must be defined, for rest
	of them its optional. (you can define them as public, private or protected
	in main game class as you wish)

	This function must be defined as per their work as specified above.

	Main game class will have the required variables for the game in it.

	Main game class will have a constructor which will call start_game() to start
	the game.

	start_game() also sets the fps and ups of the game, default value is 30 and 120
	respectively. you can also set fps and ups by directly calling set_fps() and set_ups().
	You can also access the real fps with get_fps().

	Finally you have to declare a global object of this main game class to start the game.

	Name of main game class or its object is not fixed, name them as you wish.

	This source code can have any number of other classes.

	Don't forget to put the main function the source code

	int main()
	{
		return 0;
	}

	It can be placed any where in the source code
*/
