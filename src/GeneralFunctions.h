/*
 * GeneralFunctions.h
 *
 *  Created on: Nov 8, 2020
 *      Author: edan
 */

#ifndef GENERALFUNCTIONS_H_
#define GENERALFUNCTIONS_H_

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

class GeneralFunctions {
public:
	GeneralFunctions();
	virtual ~GeneralFunctions();

	std::vector<std::string> 	split(std::string msg, char delimiter);
	std::string 				getIP();
	long int					power (int b, int e);
};

#endif /* GENERALFUNCTIONS_H_ */
