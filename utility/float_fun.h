#pragma once

#include<utility>

#include<limits>

#include<type_traits>

// source: https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/

namespace flx {

// instead of '==' or '!=' use these with floating point numbers

/*
	if the numbers are very close to 0 or 0 use the following function
	
	use std::numeric_limits<T>::epsilon() as absolute_epsilon or
	a small multiple of it
*/

template<typename T>

bool zero_eq(T a, T b, T absolute_epsilon = std::numeric_limits<T>::epsilon()) noexcept
{
	static_assert(std::is_floating_point_v<T>, "Only float, double and long double types are allowed");

	return std::abs(a - b) <= absolute_epsilon;
}

/*
	if numbers are not close to 0 use one of the following functions
*/

/*
	use std::numeric_limits<T>::epsilon() as relative_epsilon or a small multiple of it
*/

template<typename T>

bool relep_eq(T a, T b, T relative_epsilon = std::numeric_limits<T>::epsilon()) noexcept
{
	static_assert(std::is_floating_point_v<T>, "Only float, double and long double types are allowed");

	return std::abs(a - b) <= relative_epsilon * std::max(std::abs(a), std::abs(b));
}

/*
	this one is better than previous one

	adjacent floats have integer representations that are adjacent. this means that if
	we subtract the integer representations of two numbers then the difference tells us
	how far apart the numbers are in float space.

	so, we get the difference between the integer representations of the floating point
	numbers that tells us how many Units in the Last Place the numbers differ by. this
	is usually shortened to ULP.
*/

template<typename T>	/*T can be float or double*/

bool ulp_eq(T a, T b, int max_ulp_difference = 1) noexcept
{
	static_assert(std::is_floating_point_v<T> && sizeof(T) <= 8, "Only float and double types are allowed");

	int64_t ai = *(int64_t *) &a;

	int64_t bi = *(int64_t *) &b;

	return ((ai < 0) == (bi < 0)/*sign check*/) && std::abs(ai - bi) <= max_ulp_difference;
}

}	// namespace end