#pragma once

#include"../utility/pos_fun.h"
#include<vector>
#include<utility>


namespace bb
{
	class BUTTON;

	class BUTTON_LIST;
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
    
    int x, y, height, width;

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

    void set_pos(int xin, int yin, COORD_POSITION pos = TOP_LEFT)
    {
        to_top_left(this->x, this->y, xin, yin, height, width, pos);
    }

	void set_x(int xin)
    {
        this->x = xin;
    }

    void set_y(int yin)
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

    void set_height(int height)
    {
        this->height = height;
    }

    void set_width(int width)
    {
        this->width = width;
    }

	bool set_size(int height, int width)
    {
        if(height <= 0 || width <= 0)
			return false;

		this->height = height;

		this->width = width;

		return true;
    }


    public:


    // use these to get position

    void get_pos(int &xout, int &yout, COORD_POSITION pos = TOP_LEFT) const
    {
        from_top_left(xout, yout, this->x, this->y, height, width, pos);
    }

    int get_x() const
    {
        return x;
    }

    int get_y() const
    {
        return y;
    }


    // use these to get dimension

    int get_height() const
    {
        return height;
    }

    int get_width() const
    {
        return width;
    }

	void get_size(int &height, int &width) const
	{
		height = this->height;

		width = this->width;
	}


    // render proper state of the button
    
    void Render()
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

	virtual bool is_cursor_on_button(int mouse_pos_x, int mouse_pos_y)
	{
		return in_rng(x, mouse_pos_x, x + width - 1) && in_rng(y, mouse_pos_y, y + height - 1);
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

   	bool is_clicked(int mouse_pos_x, int mouse_pos_y, bool is_clicked, bool is_released)
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

	bool is_down(int mouse_pos_x, int mouse_pos_y, bool is_clicked, bool is_released)
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
	this is a general purpose prototype of menu list containing buttons in c++
	this class is not dependent on this game engine, can be used with any other
	game engine or project if needed

	it basically keeps track of a bunch of buttons (button list) created by BUTTON
	class, and lets user to select them with with keyboard as well as mouse

	How to use:

	first create several buttons (described in the description of BUTTON class)

	create an object of BUTTON_LIST and initialize them with pointers to those
	buttons

	Example: BUTTON_LIST menu({&b1, &b2, &b3, &b4});	// here b1, b2, b3, b4 are previously created button (object)

	the button pinters inserted will be stored in an array with index 0 -> (n - 1)
	n = no. of buttons

	call the public 'Update' method of this object from the update or processing
	function of the game loop, it processes and updates the list of buttons and
	the menu selector (more about menu selector below the class)

	call the public 'Render' method of this object from the render function of
	the game loop to render the menu list.

	you can use get_mbutton, get_button method or overloaded [] operator to get individual
	button as BUTTON (or its child class) object (here we return BUTTON object by reference,
	described below ...)

	we also provide is_valid_index, get_bcount, get_mindex to check an index, number of
	elements in button list and index of the button currently pointed by the selector
	respectively
*/

class bb::BUTTON_LIST
{
	/*
		its an array of pointers to button objects or buttons
	*/

	std::vector<BUTTON*> button_list;

	/*
		index of currently selected button
	*/

	int bindex;

public:

	// constructor

	/*
		takes a list of pointers to buttons and creates the button_list
	*/

	BUTTON_LIST(std::vector<BUTTON*> button_list)
	{
		this->button_list = std::move(button_list);

		bindex = 0;	// select the first button
	}

	// general methods

	/*
		check if the given index is valid or not
	*/

	bool is_valid_index(int index) const
	{
		return 0 <= index && index < button_list.size();
	}

	/*
		updates the button list, the formal parameter given to it can
		be divided into 2 parts

		#1

		int mouse_x, int mouse_y, bool is_clicked, bool is_released

		these 4 are to monitor if a button is clicked with the mouse or not
		they are compulsary

		#2

		bool up_pressed = false, bool down_pressed = false, bool enter_pressed = false

		these 3 are to accept the keyboard inputs for the button list, they
		are optional, only send arguments to them if you want to keep an option
		to select items in the menu list with keyboard

		returns the button selected or clicked else returns -1
	*/

	int Update(int mouse_x, int mouse_y, bool is_clicked, bool is_released, bool up_pressed = false, bool down_pressed = false, bool enter_pressed = false)
	{
		int button_selected = -1;

		size_t n = button_list.size();

		// button input test

		/*
			update all the button and return the clicked button
		*/

		for (int i = 0; i < n; i++)
		{
			if (button_list[i]->is_clicked(mouse_x, mouse_y, is_clicked, is_released))
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
		to render the buttons and selector on the screen
	*/

	void Render(void (*render_button_selector) (BUTTON_LIST &))
	{
		render_button_selector(*this);	// to render the menu selector

		// rendering the buttons

		for(const auto& button : button_list)
		{
			button->Render();
		}
	}

	/*
		get the no. of buttons in the button list
		bcount: button_count
	*/

	size_t get_bcount() const
	{
		return button_list.size();
	}

	/*
		get the index of the button pointed by the selector
		mindex: marked_index
	*/

	int get_mindex() const
	{
		return bindex;
	}

	/*
		Following three methods return BUTTON object by reference, that means the
		original BUTTON object which resides in the list is returned, this process
		is fast and efficient and lets you to modify the button directly, but being
		an abstract class BUTTON class can't have a physical object, actually the
		object returned here is, object of a class derived from the BUTTON class,
		induced into a BUTTON object (that means, you can only access the members
		defined in BUTTON class through this object, nothing that belongs to the
		derived class can be accessed, but the virtual methods in BUTTON class will
		be overridden or replaced by proper methods of derived class). Hence, it
		behaves just like a 'BUTTON object', though you can't create a copy of it
		by assigning it to a BUTTON object as you won't be able to create any
		BUTTON object. If you need to assign this object to some variable use
		a reference to BUTTON class. i.e.,

		BUTTON &button = get_button_pt();

		This way you can access the button returned by the method directly through
		the reference button without creating any copy of it. button will behave
		just like a BUTTON object.

		Note: We used template with next two methods so that they can return button
		object of a class derived from BUTTON
	*/

	/*
		get the BUTTON object pointed by the selector
		mbutton: marked_button

		if you want this method to return a button object of a specific type (say,
		a type or class derived from BUTTON), call it as,
		obj.get_mbutton<derived_button_type_name>()
	*/

	template<class button_type = BUTTON>

	button_type& get_mbutton() const
	{
		return *((button_type *)button_list[bindex]);
	}

	/*
		get a BUTTON object in the list

		if you want this method to return a button object of a specific type (say,
		a type or class derived from BUTTON), call it as,
		obj.get_button<derived_button_type_name>()
	*/

	template<class button_type = BUTTON>

	button_type& get_button(int i) const
	{
		return *((button_type *)button_list[i]);
	}

	/*
		overloaded [] operator to access a BUTTON object in the list
	*/

    BUTTON& operator [] (int i) const
	{
		return *(button_list[i]);
	}
};

/*
	you need to send a function pointer to the render function to print the
	selecter i.e., the symbol that indicates which menu is selected

	this function should look like

	void function_name(BUTTON_LIST &blist)
	{}

	it receives a reference to the the current BUTTON_LIST object as an argument

	this function can be inline also

	what should you define inside this fuction?

	you can use,

	BUTTON& get_mbutton() const

	(returns the button currently pointed by selector)

	overloaded [] operator or button_type& get_button(int i) const

	(returns the ith button)

	or you can use,

	int get_bcount() const

	bool is_valid_index(int index) const

	along with some code to draw or print a symbol to indicate which button or menu is selected

	!!!! never call update or render methods from this function !!!!
*/