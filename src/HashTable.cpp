/*
 * HashTable.cpp
 *
 *  Created on: Oct 23, 2020
 *      Author: edan
 */

#include "HashTable.h"

int Hash(std::string key);

/*						CONSTRUCTOR/DESTRUCTOR						*/

HashTable::HashTable()
: s_keys(std::vector<std::string>(0)), values(std::vector<std::string>(0)), size(0)
{
	std::fill (s_keys.begin(), s_keys.end(), std::string(""));
	std::fill (values.begin(), values.end(), std::string(""));
}

HashTable::~HashTable()
{

}

/*						PUBLIC						*/

// create hash table
void 				HashTable::create_table(int tableSize)
{
	s_keys = std::vector<std::string>(tableSize);
	values = std::vector<std::string>(tableSize);
	size = tableSize;
}


// insert
void 				HashTable::insert (std::string key, std::string value)
{
	uint64_t pos = Hash(key);

	if (s_keys[pos].size() != 0 || values[pos].size() != 0)
	{
		std::cout << "ERROR: collision in Hash" << std::endl;
	}
	s_keys[pos] = key;
	values[pos] = value;
}


// get
std::string			HashTable::get (std::string key)
{
	return values[Hash(key)];
}

// check if exist
bool				HashTable::is_exist(std::string key)
{
	bool exist;
	std::string value = get(key);
	value.size() ? exist =  true : exist = false;
	return exist;
}

// printf hash table
void				HashTable::show()
{
	for (int i=0; i < size; i++)
	{
		if (s_keys[i].size() > 0)
		{
			std::cout << i << ": " << s_keys[i].c_str() << "~~~ "
					<< values[i].c_str() << std::endl;
		}
	}
}

/*						PRIVATE						*/



uint64_t HashTable::StringToInt(std::string str)
{

	uint64_t res;

	GeneralFunctions g;
	for (int i=0; i<str.size(); i++)
	{
		res += uint64_t(str.c_str()[i])*g.power(10, i);
	}
	std::cout << std::endl;
	return res;
}

// get position in keys
uint64_t HashTable::Hash(std::string key)
{

	uint64_t k = StringToInt(key);
	double x = 3.15498761549;
	return ((uint64_t)(k*x)) % size;
}

