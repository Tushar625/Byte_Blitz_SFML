
#pragma once

#include"pos_fun.h"

#include"float_fun.h"

#include<algorithm>

#include<limits>

#include<numbers>

#include<concepts>

#include<type_traits>


namespace bb{

/*

	Here I provide some functions and structure to make collision detection easier
	source: https://learnopengl.com/In-Practice/2D-Game/Collisions/Collision-detection

*/

/*
	collision between two "Axis Aligned Bounded Boxes" (aabb)
	
	returns "true" => they collided, "false" => no collision
*/

inline bool aabb_collision(double x1, double y1, double width1, double height1, double x2, double y2, double width2, double height2)
{
	// if top of a box is below the bottom of other box

	if (/* box1 top */ y1 + height1 < y2 || /* box1 bottom */ y2 + height2 < y1)
	{
		return false;
	}

	// if left of a boxe is after the right of other box

	if (/* box1 left */ x1 + width1 < x2 || /* box1 right */ x2 + width2 < x1)
	{
		return false;
	}

	return true;
}

// collision between a circle and a aabb box

/*
	collision between a circle and an "Axis Aligned Bounded Boxe" (aabb)
	
	returns "true" => they collided, "false" => no collision

	also returns the point of collision via, return by reference
*/

inline bool circle_aabb_collision(double& xp, double& yp, double xc, double yc, double radius, double xb, double yb, double width, double height)
{
	coord2d<double> aabb_half = {width / 2, height / 2};

	coord2d<double> center = {xc, yc}; // center of the circle

	coord2d<double> box_center = {(xb + aabb_half.x), (yb + aabb_half.y)}; // center of the box

	// difference between the x and y coordinates of the centers of the circle and the box

	auto delta = center - box_center;

	/*
		truncating the absolute values of x and y differences to width / 2 and
		height / 2, while preserving their sign: clumping them
	*/

	delta.x = std::max(-aabb_half.x, std::min(delta.x, aabb_half.x));
	
	delta.y = std::max(-aabb_half.y, std::min(delta.y, aabb_half.y));

	// nearest point to the center of the circle on the surface of the box

	auto nearest_point = box_center + delta;	// point on the box

	xp = nearest_point.x;

	yp = nearest_point.y;

	// distance between the center of the circle and the nearest point

	auto dist = dist2d(center.x, center.y, nearest_point.x, nearest_point.y);

	return dist < radius;
}

/*
    circle_aabb_collision() returns the position of collision, following function tells,
    the point is on which side of the box

    this function returns "collision_box_side_metric" structure to indicate the side of collision
    
    (to check if the point is on a corner, say, Top Left, check if top and left both are true, in
    the returned structure)

	it also places the circle out side the aabb, by taking top left (x, y) [consider the circle is
	in a box] and diameter of the circle and modifying the top left point

	it also provides a way to put some space between the circle and the box while after collision
	position of the circle is calculated
*/

struct collision_box_side_metric
{
    bool left, right, top, bottom;
};

inline collision_box_side_metric circle_aabb_collision_side(double& x, double& y, double dia, double xp, double yp, double xb, double yb, double width, double height) noexcept
{
    collision_box_side_metric cd;
    
    /*
        as floating point numbers are not very precise we cannot check for equality
        instead we check for the difference
    */

    cd.left = flx::relep_eq(xp, xb);
    
    cd.right = flx::relep_eq(xp, xb + width);
    
    cd.top = flx::relep_eq(yp, yb);

    cd.bottom = flx::relep_eq(yp, yb + height);

	// top left

	if(cd.top && cd.left)
	{
		double dy = (y + dia - yp);	// distance between two horizontal colliding edges

		double dx = (x + dia - xp); // distance between two vertical colliding edges

		// it's a top left collision only if two distances are equal

		if(!flx::relep_eq(dy, dx))
		{
			if(dy > dx)
			{
				// left collision 

				cd.top = false;
			}
			else
			{
				// top collision

				cd.left = false;
			}
		}
	}

	// top right

	else if(cd.top && cd.right)
	{
		double dy = (y + dia - yp);

		double dx = (xp - x);

		if(!flx::relep_eq(dy, dx))
		{
			if(dy > dx)
			{
				// right collision 

				cd.top = false;
			}
			else
			{
				// top collision

				cd.right = false;
			}
		}
	}

	// bottom left

	else if(cd.bottom && cd.left)
	{
		double dy = (yp - y);

		double dx = (x + dia - xp);

		if(!flx::relep_eq(dy, dx))
		{
			if(dy > dx)
			{
				// left collision 

				cd.bottom = false;
			}
			else
			{
				// bottom collision

				cd.left = false;
			}
		}
	}

	// bottom right

	else if(cd.bottom && cd.right)
	{
		double dy = (yp - y);

		double dx = (xp - x);

		if(!flx::relep_eq(dy, dx))
		{
			if(dy > dx)
			{
				// right collision

				cd.bottom = false;
			}
			else
			{
				// bottom collision
				
				cd.right = false;
			}
		}
	}

	if (cd.left)
	{
		x = xp - dia;
	}

	if (cd.right)
	{
		x = xp;
	}

	if (cd.top)
	{
		y = yp - dia;
	}

	if (cd.bottom)
	{
		y = yp;
	}

    return cd;
}

/*
	this template function detects if a 2D point is inside a 2D polygon or not

	it takes a point and a vector of points representing a polygon, just
	remember that **** the first and last point should be same to represent
	a closed polygon

	the point should be represented by a structure with two members x and y,
	they should have same types, capable of arithmetic operations
*/

template<typename COORD2D> requires std::same_as<decltype(COORD2D::x), decltype(COORD2D::y)>&& std::is_arithmetic_v<decltype(COORD2D::x)>&& std::is_arithmetic_v<decltype(COORD2D::y)>

inline bool point_polygon_collision(const COORD2D& point, const std::vector<COORD2D>& polygon)
{
	/*
		we imagine a horizontal line point -> ray_end, with same y
	*/

	COORD2D rey_end{ std::numeric_limits<decltype(COORD2D::x)>::max(), point.y };

	/*
		we count how many times this horizontal line intersects the lines of polygon
		if the count is odd then the point is inside else the point is outside
	*/

	uint16_t intersection_count = 0;

	for (size_t i = 1; i < polygon.size(); i++)
	{
		const COORD2D& p1 = polygon[i - 1];

		const COORD2D& p2 = polygon[i];

		if ((p1.y <= point.y && point.y <= p2.y) || (p2.y <= point.y && point.y <= p1.y))
		{
			/*
				this division here must be a floating point one as it represents the
				slope of the line p1 -> p2

				so, we use float, so that all integral values will be converted to float
				but floating point values will retain their types, and the loss of precision
				for float conversion is minimal, this is also advantageous as float doesn't
				take lots of space, hence better performance or speed
			*/

			// we are calculating the x of the point of intersection

			auto intersect_x = ((float)(p2.x - p1.x) / (p2.y - p1.y)) * (point.y - p1.y) + p1.x;

			if (point.x <= intersect_x && intersect_x <= rey_end.x)
			{
				intersection_count++;
			}
		}
	}

	return intersection_count % 2;	// odd == collision
}

} // namespace bb