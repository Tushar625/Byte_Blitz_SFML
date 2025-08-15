#pragma once


#include<thread>

#include<atomic>

#include<functional>

#include<chrono>

#include<iostream>

#include<tuple>

#include"tween_accessories.h"




namespace bb
{
	class TIMER;

	class BASE_ASYNCHRONOUS_TIMER;

	class TWEENER;

	class INTERVAL_TIMER;

	class DELAY_TIMER;
}




/*
	Here's a Brief Usage Details of the class listed below

	here how final callback func should look like

	[](double dt) {}

	-~~~~~~~~~~~-
	 DELAY_TIMER
	-~~~~~~~~~~~-

    Purpose:
    --------
    Executes a callback function after a specified duration has elapsed.

    Usage:
    ------
    1. Create a DELAY_TIMER object:
         DELAY_TIMER timer;

    2. Start the timer with a duration (in seconds) and a callback:
         timer.start(2.0, [](double elapsed) {
             // Code to execute after 2 seconds
         });

	3. In your update loop, call update(dt) to update the timer:
         timer.update(dt);

	   this method calls callback function when the duration is over

    4. Check if the timer is running:
         if (timer.is_running()) { ... }

    5. Clear and stop the timer at any time:
         timer.stop();

    Notes:
    ------
    - The callback receives the actual elapsed time as its argument.
    - Once the timer fires, it automatically stops.
    - Calling start() while the timer is running has no effect.

	-~~~~~~~-
	 TWEENER
	-~~~~~~~-

	The TWEENER class provides asynchronous tweening (interpolation) of one or more variables over a
	specified duration, using customizable easing functions.

	Usage:
	------
	1. Create a TWEENER object:
		 TWEENER tween;

	2. Start tweening by calling start() with a duration, a tween tuple (or tuple list), and an
	   optional final callback:
		
		 tween.start(duration, twn(ref, start, end, ease), final_func);
		 // or for multiple variables:
		 tween.start(duration, twn_list(twn(...), twn(...)), final_func);

	   - Use twn() and twn_list() from tween_accessories.h to construct the tween tuple(s).
	   - The 'ease' parameter is optional; if omitted, the default easing function is used.

	3. To safely access tweened variables (e.g., in a render loop), use:
		 tween.lock();
		 // read or use tweened variables
		 tween.unlock();

	4. To execute the final callback (if provided) after tweening completes, call:
		 tween.xfinal();

	5. To check if the tween thread is running:
		 if (tween.is_running()) { ... }

	6. To stop the tween immediately and reset the state, don't call it in lock() - unlock() section:
		 tween.stop();

	Notes:
	------
	- The update loop runs in a separate thread; always use lock()/unlock() when accessing shared data.
	- The final callback receives the total elapsed time as its argument.
	- start() fails to execute if the tween is already running.
	- The class supports pausing and resuming via pause_time() and resume_time(), see, Pause - Resume Time
	  section of BASE_ASYNCHRONOUS_TIMER class for more details.

	-~~~~~~~~~~~~~~-
	 INTERVAL_TIMER
	-~~~~~~~~~~~~~~-

	INTERVAL_TIMER is an asynchronous timer class that repeatedly calls a user-provided callback function at
	fixed intervals, from an update loop in a separate thread. The timer continues to invoke the callback
	every interval until the callback returns false, at which point the update thread stops

	Usage:
	------
	1. Create an INTERVAL_TIMER object:
		 INTERVAL_TIMER timer;

	2. Start the timer by calling start() with the interval (in seconds), a callback, and an optional final\
	   callback:

	     double interval_sec = 1.0; // Interval in seconds

		 timer.start(interval_sec, [](double dt) -> bool {
			 // Code to execute every interval
			 // Return true to continue, false to stop
		 }, final_func);

	3. To safely access data modified by the timer (e.g., in a render loop), use lock() and unlock() as
	   with TWEENER.

	4. To execute the final callback (if provided) after the timer stops, call xfinal().

	5. Methods for checking if the timer is running, stopping the timer, and pausing/resuming time are
	   the same as in TWEENER.

	Notes:
	------
	- The update loop runs in a separate thread; always use lock()/unlock() when accessing shared data.
	- The callback receives the elapsed time for each interval as its argument and returns a boolean
	  indicating whether to continue the timer (true) or stop it (false).
	- The final callback receives the total elapsed time as its argument.
	- start() returns false if the timer is already running.
*/



/*
	this class stores a time_point, and informs how much time has elapsed
	since then in seconds.

	the internal time_point is initialized by the constructor and can be
	reset by reset().
*/

class bb::TIMER
{
	public:	


	using duration = std::chrono::duration<double>;	// represents duration in seconds using double

	using time_point = std::chrono::time_point<std::chrono::steady_clock, duration>;


	private:


	time_point clk_previous;	// storing the previous time point

	double offset;	// it is subtracted from the elapsed time to adjust the timer, can be used to pause the timer


	public:


	// constructor initializes clk_previous with current timepoint
	
	TIMER() : clk_previous{ std::chrono::steady_clock::now() }, offset{ 0 }
	{}


	// reset the timer by updating clk_previous with current timepoint

	void reset() noexcept
	{
		clk_previous = std::chrono::steady_clock::now();

		offset = 0;
	}


	// add an offset to the timer, this is useful to pause the timer

	void add_offset(double offset) noexcept
	{
		this->offset += offset;
	}

	
	/*
		retruns the time elapsed since object creation or last reset - offset in seconds.
	*/

	double elapsed_time() const noexcept
	{
		return std::chrono::duration_cast<duration>(std::chrono::steady_clock::now() - clk_previous).count() - offset;
	}
};


/*
	now I create the asynchronous timers
*/


/*
	final callback function execution system:
	-----------------------------------------
	All these asynchronous timers call a callback function after the
	timer stops. This callback function is called final()

	this class provides all the functionalities for calling the final()

	<*> brief description of final calling system:

	This asynchronous timer, when started, runs its update loop on a separate thread.
	Once this update loop finishes, the thread must call set_final() to prepare to execute
	its final() callback.

	A key challenge here is that final() itself must be executed by a different thread,
	not the same update thread that set it. To ensure this happens without race conditions,
	set_final() locks final_lock, which prevents any other update thread from setting up
	another final() before the current one is handled.

	After set_final() has secured this lock, the xfinal() method, which is called from a
	separate thread (typically an update or render thread of a game engine), runs the
	final() callback. Once final() has executed, xfinal() unlocks the final_lock, making
	it possible for future update threads to safely set up and execute their own final() callbacks.


	Start and update loop:
	----------------------
	Timer classes derived from BASE_ASYNCHRONOUS_TIMER use start() to start the timer with
	the specified parameters. If the timer is not already running, this method initializes
	the timer and begins its operation. Returns true if the timer was started successfully,
	or false if it was already active.

	start() starts a seperate thread that calls a function containing an update loop

	####################
	*** note for dev ***
	####################

	the general structure of start() method

	bool start(---the args---)
	{
		if (!thread_running)
		{
			thread_running = true;	// indicate that the update loop has started

			std::thread(---the update loop---).detach();	// calling detach() as we don't need to access this thread in future

			return true;
		}

		return false;
	}

	the general structure of the method with update loop

	update(---the args---)
	{
		---

		do
		{
			flag.wait(true);

			---
			// don't put any "break" here as the loop must
			// terminate at the while test below
			---

			flag = true;

			flag.notify_all();

		}while(---);

		---

		set_final(---);	// must call this before the thread ends
	}

	flag is a part of "Asynchronous Locking Mechanism" discussed below


	Asynchronous Locking Mechanism:
	-------------------------------
	In timer classes, the update loop modifies variables that are also accessed by other threads.
	To prevent race conditions in these cases, we implement a locking mechanism. Before any other
	thread accesses these shared variables, it should call lock() to acquire exclusive access, and call
	unlock() afterward to release it.

	The update loops of these timer objects are designed to wait for the lock to be released if another
	thread is currently holding it. Similarly, lock() itself waits for the update loop to complete its
	current iteration before granting access. This ensures that when code between lock() and unlock()
	runs, the update loop is paused, and when the update loop runs, no other thread is accessing the
	shared data. This coordinated waiting effectively prevents race conditions and keeps data access safe
	across threads.

	example:

	let the update loop of timer object "obj" is updating a varible 'x' once every second

	to print it from the render,

		obj.lock();

		// print oprtation

		obj.unlock();

	don't miss this lock() -> unlock() section, without this the update loop will pause, as a call to unlock()
	is needed to start the next iteration of the update loop

	####################
	*** note for dev ***
	####################

	an atomic variable "flag" is used, initially it's false

	each iteration of update loop waits if it's true before starting, and sets it to true at the end

	each lock() waits if it's false, and unlock() sets it to false

	so update loop waits untill unlock() sets flag to false, after that lock() waits untill update loop
	sets it back to true, thus we make sure that the update loop and lock() unlock() sections never get
	executed simultaneously

	update(---the args---)
	{
		---

		do
		{
			flag.wait(true);

			---
			// don't put any "break" here as the loop must
			// terminate at the while test below
			---

			flag = true;

			flag.notify_all();

		}while(---);

		---

		set_final(---);	// must call this before the thread ends
	}

	the structure of the update loop can be anything but the function must
	call set_final() before the thread ends

	
	Stoping this internal thread:
	-----------------------------
	I added a system to stop the update thread abruptly, just call stop() from anywhere
	except lock() - unlock() section to stop the update thread immediately without setting
	the final() callabck function.

	####################
	*** note for dev ***
	####################
	
	it internally uses a lock() - unlock() section to set stop_thread flag which is checked
	by update loop after stop() calls unlock() and stops the thread, here's how the update
	function with this stopping mechanism looks like

	update(---the args---)
	{
		---

		do
		{
			flag.wait(true);

			// #### stopping mechanism after flag wait ####

			if (stop_thread)
			{
				// break out of this thread

				thread_running = false;	// stop the update loop

				thread_running.notify_all();	// to notify stop() that the thread has stopped

				return;
			}

			---
			// don't put any "break" here as the loop must
			// terminate at the while test below
			---

			flag = true;

			flag.notify_all();

		}while(---);

		---

		set_final(---);	// must call this before the thread ends
	}

	after the thread is stopped, stop() resets all the flags and variables to their initial value

	Pause - Resume Time:
	--------------------
	When the lock() - unlock() section is not executed, the update loop gets paused, but the internal
	timer continues to run. To pause the internal timer, you can call pause_time() right before the
	loop gets paused and later call resume_time() when the loop resumes, to resume the internal timer.
	
	pause_time() tracks how long the loop is paused. When you call resume_time(), it adds this pause
	time to the internal timer as offset, which is subtracted from the elapsed time to simulate pausing
	the timer. This way, the timer continues from where it left off when the loop resumes.
*/

class bb::BASE_ASYNCHRONOUS_TIMER
{
	protected:


	// datatype to hold the callback function final()

	using final_func = std::function<void(double)>;

	// indicates if the timer is rinning or not

	std::atomic_bool thread_running;

	std::atomic_bool flag;	// used for synchronization

	std::atomic_bool stop_thread;	// to stop the update loop abruptly

	TIMER timer;	// internal timer


	private:


	TIMER pause_timer;	// used to keep track of is pause time

	std::atomic_bool pause_flag;	// indicates if the time is paused or not

	std::atomic_bool final_lock;	// to prevent race conditions while using _final or final_dt

	final_func _final;	// holds the final function

	double final_dt;	// how long the thread runs before final() is called


	protected:


	BASE_ASYNCHRONOUS_TIMER() : thread_running(false), flag(true), pause_flag(false), stop_thread(false), _final(nullptr), final_dt(0), final_lock(false)
	{}


	/*
		call it at the end of the thread to take care of thread completion and final locking
	*/

	void set_final(final_func _final, double elapsed_time) noexcept
	{
		if (_final)
		{
			/*
				after thread_running is false a thread can run quickly and reach this block.
				in such a case I wait untill the previously set final() is executed

				note: because of this mechanism if you don't execute the final function of a
				thread with xfinal, any later thread bringing a final() will get stuck here
			*/

			final_lock.wait(true);

			final_dt = elapsed_time;

			this->_final = _final;

			final_lock = true;	// signalling xfinal() that final is ready to be executed
		}

		thread_running = false;	// finally this thread can close

		thread_running.notify_all();	// to notify stop() (in case of abrupt termination) that the thread has stopped
	}


	public:


	/*
		call this where you want to execute the final() callback function.

		you can call it from update() or render(), when final() function is
		not available it doesn't do anything, after the final() function is
		set it executes and deletes it
	*/

	bool xfinal() noexcept
	{
		/*
			final() is executed only after final_lock is true (final function and it's arg is loaded)
			and the thread has stopped
		*/

		if (final_lock && !thread_running)
		{
			_final(final_dt);

			_final = nullptr;

			final_dt = 0;

			final_lock = false;

			final_lock.notify_all();

			return true;
		}

		return false;
	}
		
		
	bool is_running() const noexcept
	{
		return thread_running;
	}


	/*
		wait for the update loop to finish its current iteration
	*/

	void lock() const noexcept
	{
		if (thread_running)
		{
			flag.wait(false);
		}
	}


	/*
		we are done with the data, resume the update loop
	*/

	void unlock() noexcept
	{
		if (thread_running)
		{
			flag = false;

			flag.notify_all();
		}
	}


	/*
		it doesn't pause the timer, rather it is used to pause the internal timer object when the timer
		gets paused

		the timer gets paused if the lock() & unlock() section is not executed as that prevents the
		update loop from running

		in this case we call pause_time() to keep track of how long the timer is paused, resume_time()
		add this time to the timer as offset so that the timer can continue from where it left off
	*/

	void pause_time()
	{
		if (thread_running && !pause_flag)
		{
			pause_timer.reset();

			pause_flag = true;
		}
	}


	void resume_time()
	{
		if (thread_running && pause_flag)
		{
			timer.add_offset(pause_timer.elapsed_time());

			pause_flag = false;
		}
	}


	bool is_time_paused() const noexcept
	{
		return thread_running && pause_flag;
	}


	/*
		stop the update loop, and waits for the thread_running to turn false
		
		if the update is running this will stop the thread without setting final() function,
		but if the update loop has stopped already and set_final() is executing, final()
		stuffs will be cleared by stop() method, as it resets the flags, so you can call
		stop() at any time
		
		don't call it from in between lock() and unlock()
	*/

	void stop() noexcept
	{
		lock();

		stop_thread = true;

		unlock();

		thread_running.wait(true);	// wait for the thread to stop completely

		// resetting all the flags and variables to initial form

		flag = true;
		
		pause_flag = false;

		stop_thread = false,
			
		_final = nullptr,
			
		final_dt = 0;
		
		final_lock = false;
	}
};


/*
	This class can interpolate (or "tween") the values of several variables over a specified range
	during a given duration of time. To begin the interpolation, you call start(), which takes
	three arguments: the duration, a "tuple" describing the variables to interpolate, and a final()
	callback that is invoked once the update loop completes.

	The "tuple" argument must be created using the helper functions twn() or twn_list() provided in
	tween_accessories.h. The twn() function takes a reference to a variable, an optional start value,
	an end value, and optionally an ease function or a type of tween. It then returns a tuple containing
	all this information. The Tweener object uses this to modify the variable's value smoothly from start
	to end over the specified duration.

	If you want to interpolate multiple variables at once, you can use twn_list(). This function accepts
	several tuples created by twn() and combines them into a single tuple of tuples, which you then pass
	to start()

	example,

	let tween is an object of tweener

		tween.start(
			4,	// duration in seconds
			twn_list(
				twn(x, 0.0, (double)WIDTH - texture.getSize().x),
				twn(opac, (uint8_t)0, (uint8_t)255, TWN_TYPE::linear)
			)	// the tuple of tuples
		);

	if you have only one variable to tween, you can use the overloaded start() method

		tween.start(
			4,	// duration in seconds
			twn(x, 0.0, (double)WIDTH - texture.getSize().x)	// tween tuple
		);

	if you have several variables of same type, you can use the overloaded start() method

		tween.start(
			4,	// duration in seconds
			twn<float>({ &x, &opac }, { 0, 0 }, { (float)WIDTH - texture.getSize().x, 255 })	// tween tuple
		);

	### (x_x) ###

		twn<type>() actually takes vectors, make sure that the vectors you pass this function
		is in the scope when start() is called.

		If you call tween.start() from inside a lambda function, make sure to capture the vectors
		by value, the vectors may get destroyed before the tween starts, leading to undefined behavior.

	note:
		
		Don't forget to mention the template argument (i.e., type of the values) when
		calling this form of twn() function. More about this twn() over load in tween_accessories.h

	in case of multiple variables of same type, you can also use MULTI_TWN<T> class

		MULTI_TWN<float> twn_vector;	// MULTI_TWN object for float type

		// setting several variables for tweening

		twn_vector.set_twn(&variable1, 0.0, 100.0);

		twn_vector.set_twn(&variable2, 0.0, 100.0);

		tween.start(
			4,	// duration in seconds
			twn_vector.get_twn()	// the tuple of tuples
		);

	Here, "twn_vector.get_twn()" returns the same thing as "twn<float>({&variable1, &variable1}, {0, 0}, {100, 100})"
	with MULTI_TWN<T>, it's very easy to add several variables of same type for tweening

	### (x_x) ###

		this class creates vectors internally, make sure that object is in the scope when
		start() is called

		If you call tween.start() from inside a lambda function, make sure to capture the
		object by value, the object may go out of scope before the tween starts, leading
		to undefined behavior.

	More about MULTI_TWN<T> class in tween_accessories.h

	"TWN_TYPE::linear" is the easing function that determins the type of tween, there are 31 types of
	"tween_accessories.h"

	if you don't send the easing function to twn(), "TWN_TYPE::default_type" ("TWN_TYPE::linear" by default)
	will be used, you can change "TWN_TYPE::default_type" by simply updating it with another easing function
	from "tween_accessories.h" or something defined by you.
*/

class bb::TWEENER : public bb::BASE_ASYNCHRONOUS_TIMER
{
	// this function is run by the thread created by start()

	template <typename... TYPE>

	void update(double duration, std::tuple<TYPE...> twn_tuple, final_func _final) noexcept
	{
		double elapsed_time;

		timer.reset();

		do {
			flag.wait(true);

			if (stop_thread)
			{
				// break out of this thread

				thread_running = false;	// stop the update loop

				thread_running.notify_all();	// to notify stop() that the thread has stopped

				return;
			}

			elapsed_time = timer.elapsed_time();

			if (elapsed_time < duration)
			{
				// time is not over yet so we get the time_ratio [0 - 1]

				double time_ratio = elapsed_time / duration;

				// using apply to update all the tuples in tuple returned by twn_list()

				std::apply(

					[&time_ratio](auto&... arg) constexpr noexcept
					{
						// current = start + difference * ease(time_ratio) [ease() function returns a number between 0 - 1]

						((std::get<0>(arg) = std::get<1>(arg) + (std::get<2>(arg) - std::get<1>(arg)) * std::get<3>(arg)(time_ratio)), ...);
					},

					twn_tuple
				);
			}
			else
			{
				// duration is over so just throw in the final values

				std::apply(

					[](auto&... arg) constexpr noexcept
					{
						// current = end

						((std::get<0>(arg) = std::get<2>(arg)), ...);
					},

					twn_tuple
				);
			}

			flag = true;

			flag.notify_all();

		} while (elapsed_time < duration);

		// setting the final method before ending the thread

		set_final(_final, timer.elapsed_time());
	}


	public:


	/*
		documentation of this class explains it's usage
	*/

	template <typename... TYPE>

	bool start(double duration, std::tuple<TYPE...> twn_list, final_func _final = nullptr) noexcept
	{
		if (!thread_running)
		{
			thread_running = true;

			std::thread(&TWEENER::update<TYPE...>, this, duration, twn_list, _final).detach();

			return true;
		}

		return false;
	}

	// overloaded start() to take only one twn tuple, make it simple to deal with only one variable

	template <typename TYPE>

	bool start(double duration, std::tuple<TYPE&, TYPE, TYPE, TWN_TYPE::func> twn, final_func _final = nullptr) noexcept
	{
		return start(duration, twn_list(twn), _final);
	}

	/*
		overloaded start() to take a twn tuple consisted of vectors of pointers and values, makes
		it simple to deal with multiple variables of same type, more about this twn() overload in
		tween_accessories.h
	*/

	template <typename T>

	bool start(double duration, std::tuple<TWEEN_VECTOR_PTR<T>, TWEEN_VECTOR<T>, TWEEN_VECTOR<T>, TWN_TYPE::func> twn, final_func _final = nullptr) noexcept
	{
		return start(duration, twn_list(twn), _final);
	}
};


/*
	start() of this timer takes an interval, callback and a final callback

	the update loop calls the callback function once after each interval

	the update loop is terminated when the callback function returns false
*/

class bb::INTERVAL_TIMER : public bb::BASE_ASYNCHRONOUS_TIMER
{
	using callback_func = std::function<bool(double)>;


	void update(double required_delay, callback_func callback, final_func _final)
	{
		bool loop_continue = true;

		double total_time = 0;

		timer.reset();

		do {
			flag.wait(true);

			if (stop_thread)
			{
				// break out of this thread

				thread_running = false;	// stop the update loop

				thread_running.notify_all();

				return;
			}

			auto dt = timer.elapsed_time();

			if (dt >= required_delay)
			{
				// an interval

				timer.reset();	// restarting the timer

				total_time += dt;

				loop_continue = callback(dt);
			}

			flag = true;

			flag.notify_all();

		} while (loop_continue);

		// setting the final method before ending the thread

		set_final(_final, total_time);
	}


	public:


	/*
		documentation of this class explains it's usage
	*/

	bool start(double interval_sec, callback_func callback, final_func _final = nullptr) noexcept
	{
		if (!thread_running)
		{
			thread_running = true;

			std::thread(&INTERVAL_TIMER::update, this, interval_sec, callback, _final).detach();

			return true;
		}

		return false;
	}
};


/*
	DELAY_TIMER is a simple timer utility class that executes a user-provided callback function
	after a specified duration has elapsed.

	It is designed to be used in an update loop, where the user calls update(dt) each frame with
	the elapsed time since the last update. No multithreading involved.
	
	The timer can be started with a duration and callback using start().
	
	Once the specified duration is reached, the callback is invoked by update(dt) and the timer
	automatically stops.
	
	This class is not thread-safe and is intended for single-threaded use.
*/

class bb::DELAY_TIMER
{
	using func = std::function<void(double)>;

	double duration, elapsed_time;

	bool running = false;	// indicates if the update loop is running or not

	func callback;


public:


	DELAY_TIMER() : elapsed_time(0.0), duration(0.0), running(false), callback(nullptr)
	{
	}


	bool is_running() const noexcept
	{
		return running;
	}


	/*
		this function is called by the update loop, it updates the elapsed time
		and calls the callback function when the duration is over
	*/
	
	bool update(double dt) noexcept
	{
		if (running)
		{
			elapsed_time += dt;

			if (elapsed_time >= duration)
			{
				callback(elapsed_time);

				running = false;	// stop the update loop

				return true;	// final is executed
			}
		}

		return false;	// final is not executed
	}


	/*
		takes the duration in seconds and a callback function, the callback function
		is called by xfinal() after the duration
	*/

	bool start(double duration, func callback) noexcept
	{
		if (!running)
		{
			elapsed_time = 0;	// reset the elapsed time

			this->duration = duration;	// set the duration

			this->callback = callback;	// set the callback function

			running = true;	// indicate that the update loop has started

			return true;
		}

		return false;
	}

	
	// clear the timer and callback function

	void stop() noexcept
	{
		elapsed_time = 0.0;

		running = false;

		duration = 0.0;

		callback = nullptr;
	}
};