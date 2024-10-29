#pragma once

#include<algorithm>

#include<type_traits>


namespace bb{


/*
	general purpose position calculator functions
*/


/*
	here we have structure to represent 2D coordinates but notice
	in the FRAME_CORE class we almost never use it because,

	=> The 2D graphics engine is currently in use as a learning
	platform and prototyping and concept testing so the functions
	in it must be very portable.

	Here it is only necessary for polygon drawing as there we need
	an array of coordinates, so we have to use it.
*/

template<class type = int>
struct coord2d
{
	type x;
	type y;

	bool operator ==(coord2d pt)
	{
		return x == pt.x && y == pt.y;
	}

	bool operator !=(coord2d pt)
	{
		return x != pt.x || y != pt.y;
	}

	coord2d operator +(coord2d pt)
	{
		return coord2d(x + pt.x, y + pt.y);
	}

	coord2d operator -(coord2d pt)
	{
		return coord2d(x - pt.x, y - pt.y);
	}
};


/*
	get closest int of a floating point number
*/

template<class type_out = int, class type_in = double>

inline type_out closest(type_in x)
{
	return (x >= 0) ? (type_out)(x + .5) : (type_out)(x - .5);
}


/*
	if x is between Lrng and Hrng it gives 1 else 0
*/

template<class type>

inline bool in_rng(type Lrng, type x, type Hrng)
{
	return (x >= Lrng && x <= Hrng);
}


// 1 / sqrt() very efficient O(1) [copied]

double Q_inv_sqrt(double a)
{
    uint64_t ai;

    double _3_2 = 1.5, a_2 = a * .5;

    /*
        loading the bit representation of 'a' into 'ai'
        so that we can manipulate it
    */

    ai = *(uint64_t *) &a;

    /*
        generating floating point representation bitpattern of
        the inverse sqroot (approx) of 'a' using a spooky technique
    */

    ai = 0x5FE6F7CED916872B - (ai >> 1);

    /*
        getting back the floating point number in the bit
        pattern 'ai' i.e., inverse sqroot (approx) of 'a'
    */

    a = *(double *) &ai;

    /*
        now using newton raption formula to make 'a' more accurate
    */

    a = a * (_3_2 - a_2 * a * a);

    a = a * (_3_2 - a_2 * a * a);    // repreat it to get more accurate 'a'

    a = a * (_3_2 - a_2 * a * a);    // repreat it to get more accurate 'a'

    return a;
}


// very efficient function to get distance between 2 points

inline double dist2d(double x1, double y1, double x2, double y2)
{
	return 1 / Q_inv_sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}


/*~~~~~~ Following Functions are designed to deal with Integer type data only ~~~~~~*/


/*
	gives the length of a line whose
	starting point is a1 and ending
	point is a2 in pixels

	* * * * *
	1 2 3 4 5

	a1 = 1, a2 = 5, length = a2 - a1 + 1 = 5
*/

template <class type>

inline type get_length(type a2, type a1)
{
	return ((a2 > a1) ? (a2 - a1) : (a1 - a2)) + 1;
}

template <class type>

inline type get_width(type x2, type x1)
{
	return get_length(x2, x1);
}

template <class type>

inline type get_height(type y2, type y1)
{
	return get_length(y2, y1);
}


/*
	the below macros consider a rectangle whose height and width are known

	now they are for knowing the coordinates of some points using some
	other point

	if height or width is even like say 8, then 3 will be taken as center
	0 1 2 3 4 5 6 7

	----- always keep height or width +ve else the answer won't be correct -----
*/


/*To center*/

template <class type>

inline type get_center_x(type xl/*left*/, type width)
{
	return ((xl) + ((width) - 1) / 2);
}

template <class type>

inline type get_center_y(type yt/*top*/, type height)
{
	return get_center_x(yt, height);
}

template <class type>

inline type get_center_x_from_right(type xr/*right*/, type width)
{
	return ((xr) - (width) / 2);
}

template <class type>

inline type get_center_y_from_bottom(type yb/*bottom*/, type height)
{
	return get_center_x_from_right(yb, height);
}


/*to bottom right*/

template <class type>

inline type get_right_x(type xl/*left*/, type width)
{
	return ((xl) + (width) - 1);
}

template <class type>

inline type get_bottom_y(type yt/*top*/, type height)
{
	return get_right_x(yt, height);
}


/*to top left*/

template <class type>

inline type get_left_x(type xr/*right*/, type width)
{
	return ((xr) - (width) + 1);
}

template <class type>

inline type get_top_y(type yb/*bottom*/, type height)
{
	return get_left_x(yb, height);
}


/*from center*/

template <class type>

inline type get_left_from_center_x(type xc/*center*/, type width)
{
	return ((xc) - ((width) - 1) / 2);
}

template <class type>

inline type get_top_from_center_y(type yc/*center*/, type height)
{
	return get_left_from_center_x(yc, height);
}

template <class type>

inline type get_right_from_center_x(type xc/*center*/, type width)
{
	return ((xc) + (width) / 2);
}

template <class type>

inline type get_bottom_from_center_y(type yc/*center*/, type height)
{
	return get_right_from_center_x(yc, height);
}


template<class type>

inline type get_diameter(type radius)
{
	static_assert(std::is_integral_v<type>, "Only integral types are allowed");

	radius = abs(radius);

	return radius + radius - 1;
}


template<class type>

inline type get_radius(type diameter)
{
	static_assert(std::is_integral_v<type>, "Only integral types are allowed");

	diameter = abs(diameter);

	if((diameter & 1) == 0)
		return diameter / 2;

	return (diameter + 1) / 2;
}


// used to set the position of a sepcific coordinate in following functions

enum COORD_POSITION {TOP_LEFT, TOP_RIGHT, BOTTOM_RIGHT, BOTTOM_LEFT, CENTER, TOP_CENTER, BOTTOM_CENTER, LEFT_CENTER, RIGHT_CENTER};


/*
	(xout, yout) rpresents top left corner of the box which we get from
	(xin, yin), a position of the box as specified by 'pos'

	returns true for success false for failure
*/

template<class type>

inline bool to_top_left(type &xout, type &yout, type xin, type yin, type height, type width, COORD_POSITION pos = TOP_LEFT)
{
	static_assert(std::is_integral_v<type>, "Only integral types are allowed");

	if(height <= 0 || width <= 0)
	{
		return false;
	}

	switch (pos)
	{
		case CENTER: xout = get_left_from_center_x(xin, width); yout = get_top_from_center_y(yin, height); break;

		
		case TOP_CENTER: xout = get_left_from_center_x(xin, width); yout = yin; break;

		case BOTTOM_CENTER: xout = get_left_from_center_x(xin, width); yout = get_top_y(yin, height); break;

		
		case LEFT_CENTER: xout = xin; yout = get_top_from_center_y(yin, height); break;

		case RIGHT_CENTER: xout = get_left_x(xin, width); yout = get_top_from_center_y(yin, height); break;


		case BOTTOM_RIGHT: xout = get_left_x(xin, width); yout = get_top_y(yin, height); break;

		case BOTTOM_LEFT: xout = xin; yout = get_top_y(yin, height); break;

		
		case TOP_RIGHT: xout = get_left_x(xin, width); yout = yin; break;

		case TOP_LEFT:
		
		default: xout = xin; yout = yin; break;
	}

	return true;
}


/*
	(xin, yin) rpresents top left corner of the box from it we get 
	(xout, yout), a position of the box as specified by 'pos'

	returns true for success false for failure
*/

template<class type>

inline bool from_top_left(type &xout, type &yout, type xin, type yin, type height, type width, COORD_POSITION pos = TOP_LEFT)
{
	static_assert(std::is_integral_v<type>, "Only integral types are allowed");

	if(height <= 0 || width <= 0)
	{
		return false;
	}

	switch (pos)
	{
		case CENTER: xout = get_center_x(xin, width); yout = get_center_y(yin, height); break;

		
		case TOP_CENTER: xout = get_center_x(xin, width); yout = yin; break;

		case BOTTOM_CENTER: xout = get_center_x(xin, width); yout = get_bottom_y(yin, height); break;

		
		case LEFT_CENTER: xout = xin; yout = get_center_y(yin, height); break;

		case RIGHT_CENTER: xout = get_right_x(xin, width); yout = get_center_y(yin, height); break;


		case BOTTOM_RIGHT: xout = get_right_x(xin, width); yout = get_bottom_y(yin, height); break;

		case BOTTOM_LEFT: xout = xin; yout = get_bottom_y(yin, height); break;

		
		case TOP_RIGHT: xout = get_right_x(xin, width); yout = yin; break;

		case TOP_LEFT:
		
		default: xout = xin; yout = yin; break;
	}

	return true;
}


} // namespace bb