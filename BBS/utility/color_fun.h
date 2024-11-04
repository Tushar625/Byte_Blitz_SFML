#pragma once

#include<algorithm>



namespace bb{



/*
	general purpose rgb color handling functions
*/



/*
	here we have structure to represent rgb color but notice in the
	FRAME_CORE class we almost never use it because,

	=> The 2D graphics engine is currently in use as a learning
	platform and prototyping and concept testing so the functions
	in it must be very portable.

	Here it is only used with some functions inside or outside the
	FRAME_CORE class, internally, to make color handling operations
	easy
*/

struct rgb
{
	unsigned char r, g, b;
};



/*
	brightness functions

	the brightness is increased (amount)x

	NOTE:

	to decrease brightness n times, give 1 / n as the amount

	to increase brightness n times, give n as the amount

	if amount is 1 the brightness is left unchanged
	if amount is > 1 the brightness is increased
	if amount is < 1 the brightness is decreased

	!!!! if amount is < 0 the o/p is undefined, no protection provided, to maintain efficiency
*/

inline unsigned char brightness(unsigned char x, double amount)
{
	return std::min(255U, (unsigned)(x * amount + .5));
}

inline rgb brightness(rgb x, double amount)
{
	x.r = brightness(x.r, amount);
	x.g = brightness(x.g, amount);
	x.b = brightness(x.b, amount);

	return  x;
}



/*
	mix a source color with a background color where the source
	color has a given opacity

	this opacity must between 0 -> 1

	!!!! to maintain efficiency we don't chack for invalid opacity !!!!

	!!!! o/p is undefined for invalid opacity
*/

inline unsigned char mix_color(unsigned char source, unsigned char back_ground, double opacity)
{
	return (unsigned char)(opacity * source + (1 - opacity) * back_ground + .5);
}

inline rgb mix_color(rgb source, rgb back_ground, double opacity)
{
	source.r = mix_color(source.r, back_ground.r, opacity);
	source.g = mix_color(source.g, back_ground.g, opacity);
	source.b = mix_color(source.b, back_ground.b, opacity);

	return  source;
}

inline bool is_valid_opacity(double opacity)
{
	return (opacity >= 0 && opacity <= 1);
}



} // namespace bb