
#pragma once

#include<chrono>


namespace bb
{
	class FPS_CONTROL;
}


/*
	this class is to be used to control the fps in a loop
	say game loop

    note: this ia simpler version of the original fps control class
    here we discard all extra functionality, and keep only the necessary ones

    -> fps stabilization
    -> actual elapsed time in an iteration calculation

    why you should use it:-

    say, you have a function, if you run it multiple times, you will notice
    the time it takes to run is not consistent, it changes each time you run
    it.

    this is not good for games, if a game loop runs at arbitary speed, it will
    ruin the graphics, especially if the fps fluctuates too much, the rendering
    won't be smooth.

    to fix this issue I make this class that will stabilize the fps of a game loop
    or any kind loop, each iteration will take the same time.

    this is achieved by dynamically changing the required delay in each iteration
    of the loop

    apart from this it also gives a provision to check actual elapsed time in an
    iteration, which can be used to varify the fps also

    how to use:-

    1. declare an object of this class.

    2. you can set the fps using constructor or set() function.

    3. before entering the loop call initialize() to initialize the systems
    for calculating fps and actual elapsed time in an iteration.

    4. in the loop call put_delay() once to stabilize the fps at the value set
    before, it is best to call put_delay() the very end or the starting point
    of loop as put_delay() also calculates actual elapsed time in an iteration
    and loads it in a public member dt, which can be accessed in the game loop.
    initialize() function initializes dt to 0
    
    **** the fps can be changed from anywhere in the loop also if needed.

    !!!! note: if the system cannot provide the fps asked by the user due to
    limitations of the system where this class is used, the fps will not be
    stabilized so, set the fps according to the capability of your system
*/

class bb::FPS_CONTROL
{
    std::chrono::duration<double> required_delay, actual_delay;

    std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<double>> clk_now, clk_previous;

    public:

    double dt;

    FPS_CONTROL()
    {
        set(20);
        
        initialize();
    }

    FPS_CONTROL(double target_fps)
    {
        set(target_fps);
        
        initialize();
    }

    // set the fps

    void set(double target_fps)
    {
        required_delay = std::chrono::duration<double>{1 / target_fps};
    }

    // initialize dt and fps and time count system

    void initialize()
    {
        clk_previous = std::chrono::steady_clock::now();

        dt = 0;
    }

    /*
        implement a delay such that the fps gets close to target_fps

        by continuously changing suitable delay
    */

    /*
        gives the time passed since last call (starting from the call to initialize_dt() for first call) in seconds

        usually used to get the time between two frames in the game loop where it is called once in each iteration
    */

    /*
        these 2 functions are combined here to remove the confusion regarding which one to use
        ahead of whome

        here at first the delay is implemented and then the time elapsed in an iteration
        is calculated in seconds and stored in dt, which is kept public

        always call this function at the beginning or at the end of the game loop
    */

    void put_delay()
    {
        // adjusting suitable delay as per actual_delay & required_delay

        clk_now = std::chrono::steady_clock::now();

        /*
            time taken to print = (clk_now - clk_previous)

            current actual required delay = required_delay - time taken to print
        */

        actual_delay = (required_delay - (clk_now - clk_previous));	// in seconds

        for(clk_now = std::chrono::steady_clock::now(); (std::chrono::steady_clock::now() - clk_now) < actual_delay;);    // implementing actual delay

        // calculating actual elapsed time in an iteration

        clk_now = std::chrono::steady_clock::now();

        actual_delay = clk_now - clk_previous;

        clk_previous = clk_now;

        dt = actual_delay.count();
	}
};