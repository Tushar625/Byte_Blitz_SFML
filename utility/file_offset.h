#pragma once

#include<iostream>

/*
	Use the following class if you need to read a large structure from a file but you
	don't need to see all its data, accessing a few data members would do.

	Set the size (in bytes) of the internal buffer while creating an object.

	Read data from a file and store in into the buffer

	Write data from the buffer into a file

	Use get_data() and set_data() to read or write data in the buffer
*/


/*
	What it is ...

	This class provides an empty buffer.
	
	You have to set its size, in bytes, before using it.

	You can also destroy or deallocate the buffer if you want.

	Usage of the buffer ...

	Primary usage (reading writing):-

	After you create a buffer, several bytes long, you can read parts of a file and store
	that data inside it, or write contents inside the buffer into a file.

	Main usage (data access and manipulation):-

	Now imagine, you have a binary file where different types of data are stored, their
	sizes are also different, but you know their offset and their data types.
	
	This class is designed for such a situation, as you can access or manipulate any type
	of data at any offset in the buffer, you can either specify its data type or use a
	variable of same type to access or manipulate it.

	Thus after creating a buffer inside an object of this class, you can either fill it with
	data from a file and access them, or manually store some data in it at certain offsets,
	later you can also write the contents of this buffer into a file.

	Features of this class ...

	Constructors:
*/


class FILE_OFFSET
{
	uint8_t *buffer;	// an array where each element is 1 byte long

	size_t size;

	public:

	// constructors:-

	/*
		parameterless constructor to make the object empty
	*/

	FILE_OFFSET() : size(0), buffer(nullptr)
	{}

	/*
		parameterized constructor to set size of the buffer
		in the object while creating it
	*/

	FILE_OFFSET(size_t size) : size(0), buffer(nullptr)
	{
		set_size(size);
	}

	/*
		copy constructor
	*/

	FILE_OFFSET(FILE_OFFSET &source) : size(0), buffer(nullptr)
	{
		*this = source;
	}

	/*
		destructor
	*/

	~FILE_OFFSET()
	{
		destroy();
	}

	// overloaded operators

	/*
		overloaded '=' operator, can be used same way as normal '=' operator
		just allocate a new buffer in the lvalue object
	*/

	virtual FILE_OFFSET & operator = (const FILE_OFFSET &source)
	{
		if(buffer == source.buffer)
			return *this;

		if(!source.is_empty())
		{
			set_size(source.size);

			// source array is not empty so copying the contents

			for(auto i = 0; i < size; i++)
				buffer[i] = source.buffer[i];
		}
		else
			destroy();	// source array is empty so we destroy current object

		return *this;
	}

	/*
		overloaded '==' operator to check if two objects have same buffer
		or not (normally 2 different objects cannot have same buffer)

		this can tell if two objects are same or not as the 'size' is very
		closely related to the buffer, if buffer is empty 'size' will be 0
		so two different empty objects will produce true, which is not an
		issue
	*/

	virtual bool operator == (const FILE_OFFSET &source) const
	{
		return buffer == source.buffer;
	}

	// general utility methods

	/*
		destroys the object, doesn't cause any problem if used with empty
		object
	*/

	virtual void destroy()
	{
		delete[] buffer;

		buffer = nullptr;

		size = 0;
	}

	virtual bool is_empty() const
	{
		return (buffer == nullptr || size == 0);
	}

	/*
		creates a new 'size' bytes long buffer destroying or deallocating
		the previous buffer, and initializes new buffer with 0

		returns true if size is valid and process succeeded
		returns false if size is invalid and the process failed
	*/

	virtual bool set_size(size_t size)
	{
		if(size <= 0)
			return false;

		destroy();

		buffer = new uint8_t [size] {0};

		this->size = size;

		return true;
	}

	virtual size_t get_size() const
	{
		return size;
	}

	/*
		reads data from file pointed by 'ptr' and loads it into the buffer
		if the buffer is 'n' bytes long, 'n' bytes are read from the file

		returns true for success
		returns false for failure
	*/

	bool read(FILE *ptr)
	{
		return fread(buffer, size, 1, ptr) == 1;
	}

	/*
		writes the data inside the buffer into the file pointed by 'ptr'
		if the buffer is 'n' bytes long, 'n' bytes are written into the file

		returns true for success
		returns false for failure
	*/

	bool write(FILE *ptr) const
	{
		return fwrite(buffer, size, 1, ptr) == 1;
	}

	/*
		read or write a data from or to the buffer at a certain offset (in bytes) the
		data type of the data will be equal to data type of formal argument 'data'

		!!!!! use the offset carefully, there is no safety !!!!!
	*/

	template <class type>

	void set_data(size_t offset, type data)
	{
		/*
			get the address of data at given offset and type cast the address into proper
			pointer type and manipulate its content
		*/

		*(type *) (buffer + offset) = data;
	}

	template <class type>

	void get_data(size_t offset, type &data) const
	{
		/*
			get the address of data at given offset and type cast the address into proper
			pointer type and extract its content
		*/

		data = *(type *) (buffer + offset);
	}

	/*
		this is usefull if you know the data type but don't want use a variable

		eg. if you want to access an 'int' at offset '8' and store in 'x'

		x = onject_name.data<int>(8)
	*/

	template<class type>
	
	type & data(size_t offset)
	{
		/*
			get the address of data at given offset and type cast the address into proper
			pointer type and extract its content
		*/

		return *(type *) (buffer + offset);
	}
};