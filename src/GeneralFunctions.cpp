/*
 * GeneralFunctions.cpp
 *
 *  Created on: Nov 8, 2020
 *      Author: edan
 */

#include "GeneralFunctions.h"


GeneralFunctions::GeneralFunctions()
{

}

GeneralFunctions::~GeneralFunctions()
{

}


std::vector<std::string> GeneralFunctions::split(std::string msg, char delimiter)
{

	std::vector<std::string> 	splitted(0);
	int 						deliPosition = 0;
	std::string 				token;
	std::string					temp = msg;

	while(temp.size() > 0)
	{

		// find delimiter
		deliPosition = temp.find(delimiter);
		if (deliPosition == -1)
		{
			splitted.push_back(temp);
			break;
		}

		// take string until dlimiter
		token = temp.substr(0, deliPosition);

		// push to vector
		splitted.push_back(token);

		// delete string that was took
		temp = temp.substr(deliPosition + 1, temp.size() - deliPosition);
	}

	return splitted;
}


long int GeneralFunctions::power(int b, int e)
{
	int res = 1;
	for (int i=0; i<e; i++)
	{
		res *= b;
	}
	return res;
}

std::string GeneralFunctions::getIP()
{
	char *IPbuffer;
	struct hostent* host_enrty;
	char host[256];
	int hostname;

	hostname = gethostname(host, sizeof(host));
	if (hostname == -1)
	{
		std::cout << "ERROR: cannot get host name" << std::endl;
		return std::string("");
	}

	host_enrty = gethostbyname(host);
	if (host_enrty == NULL)
	{
		std::cout << "ERROR: cannot get host entry" << std::endl;
		return std::string("");
	}

	IPbuffer =inet_ntoa(*((struct in_addr*)host_enrty->h_addr_list[0]));
	return std::string(IPbuffer);


}
