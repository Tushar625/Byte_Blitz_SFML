
#pragma once

#include"pos_fun.h"

#include<algorithm>

#include<limits>


namespace bb{

/*

	Here I provide some functions and structure to make collision detection easier

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

} // namespace bb