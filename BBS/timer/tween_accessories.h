#pragma once

#include<tuple>

#include<cmath>

#include<numbers>

#include<functional>


namespace bb {	


/*
	these functions can be optimized with inverse_sqrt() trick
*/

// source: https://easings.net/

struct TWN_TYPE
{
	using func = std::function<double(double)>;


	// linar


	static constexpr double linear(double x) noexcept
	{
		return x;
	}


	// sine


	static double in_sine(double x) noexcept
	{
		return 1.0 - std::cos((x * std::numbers::pi) / 2.0);
	}

	static double out_sine(double x) noexcept
	{
		return std::sin((x * std::numbers::pi) / 2.0);
	}

	static double in_out_sine(double x) noexcept
	{
		return -(std::cos(std::numbers::pi * x) - 1.0) / 2.0;
	}


	// quad


	static double in_quad(double x) noexcept
	{
		return x * x;
	}

	static double out_quad(double x) noexcept
	{
		return 1.0 - (1.0 - x) * (1.0 - x);
	}

	static double in_out_quad(double x) noexcept
	{
		return (x < 0.5)
			? 2.0 * x * x
			: 1.0 - std::pow(-2.0 * x + 2.0, 2.0) / 2.0;
	}


	// cubic


	static double in_cubic(double x) noexcept
	{
		return x * x * x;
	}

	static double out_cubic(double x) noexcept
	{
		return 1.0 - std::pow(1.0 - x, 3.0);
	}

	static double in_out_cubic(double x) noexcept
	{
		return (x < 0.5)
			? 4.0 * x * x * x
			: 1.0 - std::pow(-2.0 * x + 2.0, 3.0) / 2.0;
	}


	// quart


	static double in_quart(double x) noexcept
	{
		return x * x * x * x;
	}

	static double out_quart(double x) noexcept
	{
		return 1.0 - std::pow(1.0 - x, 4.0);
	}

	static double in_out_quart(double x) noexcept
	{
		return (x < 0.5)
			? 8.0 * x * x * x * x
			: 1.0 - std::pow(-2.0 * x + 2.0, 4.0) / 2.0;
	}


	// quint


	static double in_quint(double x) noexcept
	{
		return x * x * x * x * x;
	}

	static double out_quint(double x) noexcept
	{
		return 1.0 - std::pow(1.0 - x, 5.0);
	}

	static double in_out_quint(double x) noexcept
	{
		return (x < 0.5)
			? 16.0 * x * x * x * x * x
			: 1.0 - std::pow(-2.0 * x + 2.0, 5.0) / 2.0;
	}


	// expo


	static double in_expo(double x) noexcept
	{
		return (x == 0.0)
			? 0.0
			: std::pow(2.0, 10.0 * x - 10.0);
	}

	static double out_expo(double x) noexcept
	{
		return (x == 1.0)
			? 1.0
			: 1.0 - std::pow(2.0, -10.0 * x);
	}

	static double in_out_expo(double x) noexcept
	{
		return (x == 0.0)
			? 0.0
			: (x == 1.0)
			? 1.0
			: (x < 0.5)
			? std::pow(2.0, 20.0 * x - 10.0) / 2.0
			: (2.0 - std::pow(2.0, -20.0 * x + 10.0)) / 2.0;
	}


	// circ


	static double in_circ(double x) noexcept
	{
		return 1.0 - std::sqrt(1.0 - std::pow(x, 2.0));
	}

	static double out_circ(double x) noexcept
	{
		return std::sqrt(1.0 - std::pow(x - 1.0, 2.0));
	}

	static double in_out_circ(double x) noexcept
	{
		return (x < 0.5)
			? (1.0 - std::sqrt(1.0 - std::pow(2.0 * x, 2.0))) / 2.0
			: (std::sqrt(1.0 - std::pow(-2.0 * x + 2.0, 2.0)) + 1.0) / 2.0;
	}


	// back


	static double in_back(double x) noexcept
	{
		constexpr double c1 = 1.70158;
		constexpr double c3 = c1 + 1.0;

		return c3 * x * x * x - c1 * x * x;
	}

	static double out_back(double x) noexcept
	{
		constexpr double c1 = 1.70158;
		constexpr double c3 = c1 + 1.0;

		return 1.0 + c3 * std::pow(x - 1.0, 3.0) + c1 * std::pow(x - 1.0, 2.0);
	}

	static double in_out_back(double x) noexcept
	{
		constexpr double c1 = 1.70158;
		constexpr double c2 = c1 * 1.525;

		return (x < 0.5)
			? (std::pow(2.0 * x, 2.0) * ((c2 + 1.0) * 2.0 * x - c2)) / 2.0
			: (std::pow(2.0 * x - 2.0, 2.0) * ((c2 + 1.0) * (2.0 * x - 2.0) + c2) + 2.0) / 2.0;
	}


	// elastic


	static double in_elastic(double x) noexcept
	{
		constexpr double c4 = (2.0 * std::numbers::pi) / 3.0;

		return (x == 0.0)
			? 0.0
			: (x == 1.0)
			? 1.0
			: -std::pow(2.0, 10.0 * x - 10.0) * std::sin((x * 10.0 - 10.75) * c4);
	}

	static double out_elastic(double x) noexcept
	{
		constexpr double c4 = (2.0 * std::numbers::pi) / 3.0;

		return (x == 0.0)
			? 0.0
			: (x == 1.0)
			? 1.0
			: std::pow(2.0, -10.0 * x) * std::sin((x * 10.0 - 0.75) * c4) + 1.0;
	}

	static double in_out_elastic(double x) noexcept
	{
		constexpr double c5 = (2.0 * std::numbers::pi) / 4.5;

		return (x == 0.0)
			? 0.0
			: (x == 1.0)
			? 1.0
			: (x < 0.5)
			? -std::pow(2.0, 20.0 * x - 10.0) * std::sin((20.0 * x - 11.125) * c5) / 2.0
			: std::pow(2.0, -20.0 * x + 10.0) * std::sin((20.0 * x - 11.125) * c5) / 2.0 + 1.0;
	}


	// bounce


	static double out_bounce(double x) noexcept
	{
		constexpr double n1 = 7.5625;
		constexpr double d1 = 2.75;

		if (x < 1.0 / d1)
		{
			return n1 * x * x;
		}
		else if (x < 2.0 / d1)
		{
			x -= 1.5 / d1;
			return n1 * x * x + 0.75;
		}
		else if (x < 2.5 / d1)
		{
			x -= 2.25 / d1;
			return n1 * x * x + 0.9375;
		}
		else
		{
			x -= 2.625 / d1;
			return n1 * x * x + 0.984375;
		}
	}

	static double in_bounce(double x) noexcept
	{
		return 1.0 - out_bounce(1.0 - x);
	}

	static double in_out_bounce(double x) noexcept
	{
		return (x < 0.5)
			? (1.0 - out_bounce(1.0 - 2.0 * x)) / 2.0
			: (1.0 + out_bounce(2.0 * x - 1.0)) / 2.0;
	}


	// initializing default type

	inline static func default_type = linear;
};


/*
	The twn() function takes a reference to a variable, an optional start value, an end value, and
	optionally an ease function or a type of tween. It then returns a tuple containing all this
	information. The Tweener object uses this to modify the variable's value smoothly from start to
	end over the specified duration.
*/

template <typename TYPE>

auto twn(TYPE& data, TYPE end, TWN_TYPE::func ease = nullptr) noexcept
{
	// assuming that data is already initialized

	if (!ease)
	{
		ease = TWN_TYPE::default_type;
	}

	return std::tuple<TYPE&, TYPE, TYPE, TWN_TYPE::func>(
		data	/*ref*/,
		data	/*initial value*/,
		end		/*end value*/,
		ease	/*easing function*/
	);
}


template <typename TYPE>

auto twn(TYPE& data, TYPE start, TYPE end, TWN_TYPE::func ease = nullptr) noexcept
{
	data = start;	// initialize data with start

	return twn(data, end, ease);
}


/*
	This function accepts several tuples created by twn() and combines them into a
	single tuple of tuples
*/

template <typename... TYPE>

auto twn_list(TYPE... twn_tuple) noexcept
{
	return std::tuple<TYPE...>(twn_tuple...);
}


/*
	Tollowing two classes are used to wrap around std::vector<T> and std::vector<T*>
	so that we can perform element-wise arithmetic operations on two std::vector<T>
	and assign resulting values to the variables pointed by the pointers in std::vector<T*>

	This way I want to tween multiple values of same type at once and we don't have to
	know the exact number of variables to tween at compile time
*/


// declaring TWEEN_VECTOR_PTR class so that we can make it a friend of TWEEN_VECTOR class

template<typename T>
class TWEEN_VECTOR_PTR;


/*
	the TWEEN_VECTOR class is a wrapper around std::vector<T>, that allows for element-wise
	arithmetic operations between two TWEEN_VECTOR objects or between a TWEEN_VECTOR and a
	scalar value.
*/

template<typename T>

class TWEEN_VECTOR
{

	std::vector<T> data;

	friend class TWEEN_VECTOR_PTR<T>;


public:

	
	// Constructor

	TWEEN_VECTOR(const std::vector<T>& vec) : data(vec) {}

	
	// Arithmetic operators (TWEEN_VECTOR op TWEEN_VECTOR) (element-wise)

	TWEEN_VECTOR operator+(const TWEEN_VECTOR& other) const
	{
		return elementwise_op(other, std::plus<T>{});
	}

	TWEEN_VECTOR operator-(const TWEEN_VECTOR& other) const
	{
		return elementwise_op(other, std::minus<T>{});
	}

	TWEEN_VECTOR operator*(const TWEEN_VECTOR& other) const
	{
		return elementwise_op(other, std::multiplies<T>{});
	}

	TWEEN_VECTOR operator/(const TWEEN_VECTOR& other) const
	{
		return elementwise_op(other, std::divides<T>{});
	}


	// Arithmetic operators (TWEEN_VECTOR op value) (element-wise)
	
	TWEEN_VECTOR operator*(double value) const
	{
		return elementwise_op(value, std::multiplies<double>{});
	}


private:


	/*
		applies the element wise operations defined by the Op functor

		Here, we create a new result object instead of updating the
		existing object as we need to preserve the original objects
		during the tween operation
	*/


	// between two tween_vectors
	
	template<typename Op>
	
	TWEEN_VECTOR elementwise_op(const TWEEN_VECTOR& other, Op op) const
	{
		if (data.size() != other.data.size())
		{
			throw std::invalid_argument("Vector sizes must match for arithmetic");
		}

		std::vector<T> result;

		result.reserve(data.size());

		for (size_t i = 0; i < data.size(); ++i)
		{
			result.push_back(op(data[i], other.data[i]));
		}

		return TWEEN_VECTOR(result);
	}


	// between TWEEN_VECTOR and a value

	template<typename Op>

	TWEEN_VECTOR elementwise_op(double value, Op op) const
	{
		std::vector<T> result;

		result.reserve(data.size());

		for (size_t i = 0; i < data.size(); ++i)
		{
			result.push_back(op((double)data[i], value));
		}

		return TWEEN_VECTOR(result);
	}
};


/*
	the TWEEN_VECTOR_PTR class is a wrapper around std::vector<T*>, that allows data
	in TWEEN_VECTOR to be assigned to the variables pointed by the vector of pointers
*/

template <typename T>

class TWEEN_VECTOR_PTR
{

	std::vector<T*> data;


public:


	// constructor

	TWEEN_VECTOR_PTR(const std::vector<T*>& vec) : data(vec) {}

	// Assignment from TWEEN_VECTOR<T>

	/*
		assigns the values in the vector of TWEEN_VECTOR<T> to the variables pointed
		by the vector of pointers in TWEEN_VECTOR_PTR<T>
	*/

	void operator=(const TWEEN_VECTOR<T>& source) const
	{
		if (data.size() != source.data.size())
		{
			throw std::invalid_argument("Vector sizes must match for arithmetic");
		}

		for(unsigned long long i = 0; i < data.size(); i++)
		{
			*data[i] = source.data[i];
		}
	}
};


/*
	following two functions are overloaded versions of twn() that take

		- a vector of pointers, instead of a single reference
		- a vector of values, instead of a single value, as start and end values
		- an optional easing function as usual

	their purpose is to allow tweening of multiple variables of same type at once

	their working is logically similar to the single variable twn() functions

	you can also use twn_list() to combine several of these twn() calls into a
	single tuple, just like the twn() function for single variables

	unlike the single variable twn() functions, you must mention the template argument
	(i.e., type of the values) when calling these twn() functions

	example:

	twn<float>({ &x, &opac }, { 0, 0 }, { (float)WIDTH - texture.getSize().x, 255 });

	twn<float>({ &x, &opac }, { (float)WIDTH - texture.getSize().x, 255 });

	If only one vector values are passed, current values of the data pointed by pointers
	are used as the initial values

	note: ### (x_x) ###
	
		twn<type>() actually takes vectors, make sure that the vectors you pass this function
		is in the scope when start() is called.

		If you call tween.start() from inside a lambda function, make sure to capture the vectors
		by value, the vectors may get destroyed before the tween starts, leading to undefined behavior.
*/


template <typename T>

auto twn(const std::vector<T*>& data, const std::vector<T>& start, const std::vector<T>& end, TWN_TYPE::func ease = nullptr) noexcept
{
	// assuming that data is already initialized

	if (!ease)
	{
		ease = TWN_TYPE::default_type;
	}

	return std::tuple<TWEEN_VECTOR_PTR<T>, TWEEN_VECTOR<T>, TWEEN_VECTOR<T>, TWN_TYPE::func>(
		TWEEN_VECTOR_PTR<T>{ data }	/*pointers*/,
		TWEEN_VECTOR<T>{ start }	/*initial value*/,
		TWEEN_VECTOR<T>{ end }		/*end value*/,
		ease						/*easing function*/
	);
}


template <typename T>

auto twn(const std::vector<T*>& data, const std::vector<T>& end, TWN_TYPE::func ease = nullptr) noexcept
{
	// assuming that data is already initialized

	if (!ease)
	{
		ease = TWN_TYPE::default_type;
	}

	// Create a vector to hold the initial values

	std::vector<T> start(data.size());

	// store current values of pointers as initial values

	for (size_t i = 0; i < data.size(); ++i)
	{
		start[i] = *data[i];
	}

	return twn(data, start, end, ease);
}



/*
	this class is used to help with tweening multiple variables of same type at once

	we do have 2 overloaded twn() functions (defined above) to take care of this but
	they take vectors as arguments, which is not very easy to use

	using MULTI_TWN<T> class, you can create an object, add tween information for multiple
	variables of same type, and then call get_twn() to get a tuple (just like the one
	generated by twn() overloads), which can be passed to TWEENER::start().
	
	you can set the easing function as well

	note: ### (x_x) ###

		this class creates vectors internally, make sure that object is in the scope when
		start() is called

		If you call tween.start() from inside a lambda function, make sure to capture the
		object by value, the object may go out of scope before the tween starts, leading
		to undefined behavior.

	example:

	MULTI_TWN<float> twn_vector();

	// setting several variables for tweening

	twn_vector.set_twn(&data1, 0.0, 100.0);

	twn_vector.set_twn(&data2, 0.0, 100.0);

	twn_vector.get_twn();	// returns the tuple, an arg to tweener start() or twn_list()	
*/

template <typename T>

class MULTI_TWN
{
	std::vector<T*> pointer;	// vector of pointers to the variables to tween
	
	std::vector<T> start, end;	// initial and end values of the variables to tween
	
	TWN_TYPE::func ease;		// easing function to use for tweening


public:


	// Constructor

	/*
		initializes the easing function, and reserves space for the vectors
	*/

	MULTI_TWN(std::size_t size, TWN_TYPE::func _ease = nullptr) : ease(_ease ? _ease : TWN_TYPE::default_type)
	{
		reserve(size);
	}

	// initializes the easing function

	MULTI_TWN(TWN_TYPE::func _ease = nullptr) : ease(_ease ? _ease : TWN_TYPE::default_type)
	{}


	// Member functions to set tweening information for multiple variables

	void set_twn(T* ptr, T _start, T _end) noexcept
	{
		pointer.push_back(ptr);	// add pointer to the vector

		start.push_back(_start);	// add initial value to the vector

		end.push_back(_end);	// add end value to the vector
	}

	void set_twn(T* ptr, T end) noexcept
	{
		set_twn(ptr, *ptr, end);
	}


	// Sets the easing function to be used for tweening

	void set_ease(TWN_TYPE::func _ease) noexcept
	{
		ease = _ease;	// set the easing function
	}


	// Clears the vectors, effectively ereasing the tweening information

	void clear() noexcept
	{
		pointer.clear();

		start.clear();

		end.clear();
	}


	// Reserves space for the vectors

	void reserve(std::size_t size) noexcept
	{
		pointer.reserve(size);

		start.reserve(size);

		end.reserve(size);
	}


	// Returns a tuple containing the tweening information

	auto get_twn() const noexcept
	{
		return twn(pointer, start, end, ease);	// return the tuple
	}
};


}	// namespace bb