/*
 * HashTable.h
 *
 *  Created on: Oct 23, 2020
 *      Author: edan
 */

#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>		/* uintX_t */
#include "Defines"
#include "GeneralFunctions.h"

class HashTable {
public:
	HashTable();
	virtual ~HashTable();

	// create hash table
	void 								create_table(int size);

	// insert
	void 								insert (std::string key, std::string value);

	// get
	std::string							get (std::string key);

	// check if exist
	bool								is_exist(std::string key);

	// printf hash table
	void								show();

private:

	std::vector<std::string> 			s_keys;
	std::vector<std::string> 			values;
	int									size;

	// get position in keys
	uint64_t 							Hash(std::string key);

	// convert string to int
	uint64_t							StringToInt(std::string str);

};

#endif /* HASHTABLE_H_ */
