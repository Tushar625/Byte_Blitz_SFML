#pragma once

#include"../utility/pos_fun.h"
#include<vector>
#include<utility>
#include<functional>


namespace bb
{
	class BUTTON;

	template<typename BUTTON_TYPE>

	class MENU;
}


/*
    this is a general purpose prototype of button in c++
    this is made to be compatible with different game engines as it never contains any
    engine specific code

    to use it, simply make a class for the type of button you want to use and
    inherit the properties of button from this class "public inheritance"

    **** in the derived class set the size of the button while creating it.

    redefine the sates of the button functions, is_cursor_on_button() [if required] and
	you are done.

    now declare objects of this button to use it after setting its position and size properly
	(these are usually set in an initialize function or constructor if possible)

    in the render function of the game loop keep the rendering function that
    will display the proper state of the button

    and keep the is_clicked or is_down function in processing or update function of game loop
    to get the i/p and change the states of the button
*/


// main general purpose button class

class bb::BUTTON
{
    private:

    /*
        enum to discribe the states of the button

        this has no use outside the class currently so, we keep it in the class
        if it's needed outside in future we will put it outside the class, in its
        previous position

        !!!! Note that, you cannot access these data members directly from child class
    */

    enum BUTTON_STATE {ORDINARY/*cursor outside button*/, HOVERING/*cursor on the button but no click*/, CLICK/*cursor on the button and clicked*/};

    // button data
    
    float x, y, height, width;

	BUTTON_STATE b_state;


    public:


    // basic constructor sets button data to the initial value

    BUTTON() : b_state(ORDINARY)
    {
        x = y = height = width = 0;
    }


    /*
        set the position of the button

        xin, yin represent a position of the button according to pos,
        it can be TOP_LEFT, CENTER, BOTTOM_RIGHT according to value
        of pos.

        at the end this posittion is converted to top left position
        of the button and saved as x, y in the object

        !!!!! Note that height and width must be set properly before calling set_pos 
        else x, y won't be calculated accurately!!!!!
    */

    void set_pos(float xin, float yin, COORD_POSITION pos = TOP_LEFT) noexcept
    {
        to_top_left(this->x, this->y, xin, yin, height, width, pos);
    }

	void set_x(float xin) noexcept
    {
        this->x = xin;
    }

    void set_y(float yin) noexcept
    {
        this->y = yin;
    }


    protected:


    /*
        these 2 mutator methods are kept to let user modify these
        2 variables from child class without directly accessing them

        they are kept protected as I don't want user to change the dimension
        of a button from its object
    */


    // use these to set dimension

    void set_height(float height) noexcept
    {
        this->height = height;
    }

    void set_width(float width) noexcept
    {
        this->width = width;
    }

	bool set_size(float height, float width) noexcept
    {
        if(height <= 0 || width <= 0)
			return false;

		this->height = height;

		this->width = width;

		return true;
    }


    public:


    // use these to get position

    void get_pos(float& xout, float& yout, COORD_POSITION pos = TOP_LEFT) const noexcept
    {
        from_top_left(xout, yout, this->x, this->y, height, width, pos);
    }

	float get_x() const noexcept
    {
        return x;
    }

	float get_y() const noexcept
    {
        return y;
    }


    // use these to get dimension

	float get_height() const noexcept
    {
        return height;
    }

	float get_width() const noexcept
    {
        return width;
    }

	void get_size(float& height, float& width) const noexcept
	{
		height = this->height;

		width = this->width;
	}


    // render proper state of the button
    
    void Render() noexcept
    {
        switch(b_state)
        {
            case ORDINARY: ORDINARY_STATE(); break;

            case HOVERING: HOVERING_STATE(); break;

            case CLICK: CLICK_STATE(); break;
        }
    }


	protected:


    // virtual function for ORDINARY state

    virtual void ORDINARY_STATE() = 0;


    // virtual function for HOVERING state

    virtual void HOVERING_STATE()
    {
        ORDINARY_STATE();
    }


    // virtual function for CLICK state

    virtual void CLICK_STATE()
    {
        HOVERING_STATE();
    }


	// virtual function to check if the cursor is on the button not

	virtual bool is_cursor_on_button(float mouse_pos_x, float mouse_pos_y) const noexcept
	{
		/*
			in a float system there can be points in between x + width - 1 and x + width
			we don't know how many points are there so, we check for < x + width not
			<= x + width - 1, same goes for y axis
		*/
		
		return in_rng_open(x, mouse_pos_x, x + width) && in_rng_open(y, mouse_pos_y, y + height);
	}


	public:


    /*
        this is the heart of this button

        it takes:-

        current position of the cursor on the screen
        and status of a mouse button (i.e., whether its
        clicked or released)

        based on these it gives:-

        current state of the button to render it
        it its completely clicked or not (i.e., if the
        cursor moves away from the button while its
        clicked then its not complete click)
    */

   	/*
		The complete click concept introduced here basically means the pointer must not
		move from the button when it is clicked, only then we signal a button click after
		the button is released
	*/

   	bool is_clicked(float mouse_pos_x, float mouse_pos_y, bool is_clicked, bool is_released) noexcept
	{
		/*
			here we implement the buttons using finite state machine, as its a lot more
			easier to understand, make, debug and modify, as the transition between the
			three states of the button is clearly visible here

			Now, I think this is the best way to do it.
		*/
		
		switch(b_state)
		{
			case ORDINARY:

				// we can go to Hovering and Click state from here

				if(is_cursor_on_button(mouse_pos_x, mouse_pos_y)/*mouse pointer on the button*/)
				{
					if(is_clicked)
						b_state = CLICK;
					else
						b_state = HOVERING;
				}

				break;

			case HOVERING:
				
				// we can go to Ordinary and Click state from here

				if(is_cursor_on_button(mouse_pos_x, mouse_pos_y)/*mouse pointer on the button*/)
				{
					if(is_clicked)
						b_state = CLICK;
				}
				else
					b_state = ORDINARY;

				break;

			case CLICK:

				// we can go to Ordinary and Hovering state from here

				if(is_cursor_on_button(mouse_pos_x, mouse_pos_y)/*mouse pointer on the button*/)
				{
					if(is_released)
					{
						/*
							the pressed button must be released without moving the
							mouse pointer from the button
						*/

						b_state = HOVERING;

						return true;	// complete click detected
					}
				}
				else
					b_state = ORDINARY;

				break;
		}

		return false;
	}


    /*
        this function checks the state of the button
        if the state of the button is CLICK it returns true else false is returned

        note that it does not check for complete clicks, rather it gives true as
        long as the mouse button is down, even if the pointer is out side the button
        it only becomes false after the key is released
    */

	bool is_down(float mouse_pos_x, float mouse_pos_y, bool is_clicked, bool is_released) noexcept
	{
		/*
			here we implement the buttons using finite state machine, as its a lot more
			easier to understand, make, debug and modify, as the transition between the
			three states of the button is clearly visible here

			Now, I think this is the best way to do it.
		*/
		
		switch(b_state)
		{
			case ORDINARY:

				// we can go to Hovering and Click state from here

				if(is_cursor_on_button(mouse_pos_x, mouse_pos_y)/*mouse pointer on the button*/)
				{
					if(is_clicked)
						b_state = CLICK;
					else
						b_state = HOVERING;
				}

				break;

			case HOVERING:
				
				// we can go to Ordinary and Click state from here

				if(is_cursor_on_button(mouse_pos_x, mouse_pos_y)/*mouse pointer on the button*/)
				{
					if(is_clicked)
						b_state = CLICK;
				}
				else
					b_state = ORDINARY;

				break;

			case CLICK:

				// we can go to Ordinary and Hovering state from here

				if(is_released)
				{
					/*
						the pressed button must be released to go to ordinary or hovering state
					*/
					
					if(is_cursor_on_button(mouse_pos_x, mouse_pos_y)/*mouse pointer on the button*/)
						b_state = HOVERING;
					else
						b_state = ORDINARY;
				}

				break;
		}

		if(b_state == CLICK)
            return true;
        else
            return false;
	}
};


/*
	-----------------
	Menu System (C++)
	-----------------

	This is a general-purpose prototype of a menu system for managing a group of buttons in C++.
	The class is independent of the game engine, meaning it can be reused in other projects or engines.

	It manages a set of buttons of the same type (i.e., derived from the BUTTON base class) and
	allows users to select them using both keyboard and mouse input.

	----------
	How to Use
	----------

	1. Create a MENU object and initialize it with any number of button objects of the same type:

		MENU menu({b1, b2, b3, b4}); // b1, b2, b3, b4 are buttons of same type

		The buttons will be stored in an internal array with indices ranging from 0 to n - 1,
		where n is the number of buttons.

	2. Call Update() in your game loop's update or processing function:

		* Handles both mouse and keyboard input

		* Updates the internal state of buttons

		* Returns the index of the selected button, or -1 if none is selected

	3. Call Render() in your render function to draw the menu and all buttons.

	----------------
	Member Functions
	----------------

	* get_mbutton() - access the button currently pointed to by the selector
	
	* menu[i] - access ith button

	* is_valid_index(i) - check if an index is within bounds

	* get_bcount() - get the total number of buttons

	* get_mindex() - get the index currently pointed to by the selector
	
	-------------------------
	Custom Selector Rendering
	-------------------------

	To render a selector (a symbol indicating the currently selected menu item),
	you must pass a function to the Render() method.

	This function should follow the signature:

		void function_name(MENU& menu)
		{
			// use menu.get_mindex() to access currently selected button draw the selector
		}

		* It receives the current MENU object by reference.
		
		* Use its methods to access the currently selected index and draw the selector accordingly.

	!!!! Important !!!!
	
	Do not call Update() or Render() from within this function, doing so will cause undefined behavior.
*/

template<typename BUTTON_TYPE>

class bb::MENU
{
	/*
		vector of buttons
	*/

	std::vector<BUTTON_TYPE> button_list;

	/*
		index of currently selected button
	*/

	int bindex;

public:

	// constructor

	/*
		takes a vector of buttons, creates the button_list and selects the first button

		----------------------------------------------------------------------------------------
		here's why we use pass-by-value for the button list, instead of const pass-by-reference:
		----------------------------------------------------------------------------------------

		"The pass-by-value advice is suitable only for parameters that the function would copy anyway.
		In that case, by using pass-by-value semantics, the code is optimal for both lvalues and rvalues.
		If an lvalue is passed in, it's copied exactly one time, just as with a reference-to-const parameter.
		And, if an rvalue is passed in, no copy is made, just as with an rvalue reference parameter."

		page 305, Professional C++ by Marc Gregoire, Nicholas A. Solter, Scott Meyers
	*/

	MENU(std::vector<BUTTON_TYPE> _button_list) :
		button_list(std::move(_button_list)),
		bindex(0)	// select the first button
	{}

	// general methods

	/*
		check if the given index is valid or not
	*/

	bool is_valid_index(int index) const noexcept
	{
		return 0 <= index && index < button_list.size();
	}

	/*
		updates the button list, the formal parameter given to it can
		be divided into 2 parts

		#1 Mouse Input

		int mouse_x, int mouse_y, bool is_clicked, bool is_released

		these 4 are to monitor if a button is clicked with the mouse or not
		they are compulsary

		#2 Keyboard Input

		bool up_pressed = false, bool down_pressed = false, bool enter_pressed = false

		these 3 are to accept the keyboard inputs for the button list, they
		are optional, only send arguments to them if you want to keep an option
		to select items in the menu list with keyboard

		returns the button selected or clicked else returns -1
	*/

	int Update(float mouse_x, float mouse_y, bool is_clicked, bool is_released, bool up_pressed = false, bool down_pressed = false, bool enter_pressed = false) noexcept
	{
		int button_selected = -1;

		size_t n = button_list.size();

		// button input test

		/*
			update all the button and return the clicked button
		*/

		for (int i = 0; i < n; i++)
		{
			if (button_list[i].is_clicked(mouse_x, mouse_y, is_clicked, is_released))
			{
				button_selected = i;
			}
		}

		// keyboard input test

		if (up_pressed)
		{
			// we select the previous button

			bindex--;

			if (bindex < 0)
				bindex = static_cast<int>(n - 1);
		}

		if (down_pressed)
		{
			// we select the next button

			bindex++;

			if (bindex == n)
				bindex = 0;
		}

		if (enter_pressed)
		{
			// if enter pressed we return the index of currently selected button

			button_selected = bindex;
		}

		return button_selected;
	}

	/*
		renders the buttons and selector on the screen

		it takes a function that takes a reference to the current MENU object as an argument
		and renders the selector on the screen
	*/

	void Render(const std::function<void(MENU&)> render_button_selector) noexcept
	{
		render_button_selector(*this);	// to render the menu selector

		// rendering the buttons

		for (auto& button : button_list)
		{
			button.Render();
		}
	}

	/*
		get the no. of buttons in the button list
		bcount: button_count
	*/

	size_t get_bcount() const noexcept
	{
		return button_list.size();
	}

	/*
		get the index of the button pointed by the selector
		mindex: marked_index
	*/

	int get_mindex() const noexcept
	{
		return bindex;
	}

	/*
		get the BUTTON_TYPE object pointed by the selector
		mbutton: marked_button
	*/

	BUTTON_TYPE& get_mbutton() noexcept
	{
		return button_list[bindex];
	}

	/*
		overloaded [] operator to access a BUTTON_TYPE object in the list
	*/

	BUTTON_TYPE& operator [] (int i) noexcept
	{
		return button_list[i];
	}
};