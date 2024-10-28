
#pragma once

#include"pos_fun.h"

#include<algorithm>

// collision between two aabb boxes

inline bool aabb_collision(double x1, double y1, double width1, double height1, double x2, double y2, double width2, double height2)
{
	// if top of a box is below the bottom of other box

	if (/* top */ y1 + height1 < y2 || /* bottom */ y2 + height2 < y1)
	{
		return false;
	}

	// if left of a boxe is after the right of other box

	if (/* left */ x1 + width1 < x2 || /* right */ x2 + width2 < x1)
	{
		return false;
	}

	return true;
}

// collision between a circle and a aabb box

inline bool circle_aabb_collision(double& xp, double& yp, double xc, double yc, double radius, double xb, double yb, double width, double height)
{
	coord2d<double> aabb_half = {width / 2, height / 2};

	coord2d<double> center = {xc, yc}; // center of the circle (ball)

	coord2d<double> box_center = {(xb + aabb_half.x), (yb + aabb_half.y)}; // center of the box

	auto delta = center - box_center;

	delta.x = std::max(-aabb_half.x, std::min(delta.x, aabb_half.x));
	
	delta.y = std::max(-aabb_half.y, std::min(delta.y, aabb_half.y));

	auto nearest_point = box_center + delta;	// point on the box

	// nearest point on the serface of the box

	xp = nearest_point.x;

	yp = nearest_point.y;

	// delta = nearest_point - center; // distance between P and C

	auto dist = dist2d(center.x, center.y, nearest_point.x, nearest_point.y);

	return dist < radius;
}

// circle_aabb_collision() returns the position of collision, following function tells the point is on which side of the box

struct collision_box_side_metric
{
	bool left, right, top, bottom;
};

inline collision_box_side_metric circle_aabb_collision_side(double xp, double yp, double xb, double yb, double width, double height, double difference = .0001)
{
	collision_box_side_metric cd;
	
	/*
		as floating point numbers are not very precise we cannot check for equality
		instead we check for the difference
	*/

	cd.left = std::abs(xp - xb) < difference;
	
	cd.right = std::abs(xp - xb - width) < difference;
	
	cd.top = std::abs(yp - yb) < difference;

	cd.bottom = std::abs(yp - yb - height) < difference;

	return cd;
}