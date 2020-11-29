/*
 * Function.h
 *
 *  Created on: Nov 26, 2020
 *      Author: edan
 */

#ifndef FUNCTION_H_
#define FUNCTION_H_

#include "Defines"
#include <iostream>
#include <string.h>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>		/* uintX_t */
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class Function {
public:
	Function();
	virtual ~Function();

	std::vector<std::string> 	split(std::string msg, char delimiter);
	std::string 				getIP();
	long int					power (int b, int e);
};

#endif /* FUNCTION_H_ */
