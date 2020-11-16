/*
 * ConsistingHashing.h
 *
 *  Created on: Oct 24, 2020
 *      Author: edan
 */

#ifndef CONSISTINGHASHING_H_
#define CONSISTINGHASHING_H_

#include <stdio.h>			/* itoa */
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>			/* atoi */
#include "TCPConnection.h"
#include "Defines"

//#define VIRTUAL_NAMES				3
//#define HASH_DOUBLE_VALUE			15687.3378107

class ConsistingHashing
{
private:

	std::vector<std::string> 	ring;
	int							size;
	int 						occ_spots;	// occupied spots

	// hash function for full server names (including the virtual/original bit)
	int							HashName(std::string fullServerName);

	// generate server's name
	void						GenerateServerName(std::string serverInfo, std::vector<int> *names);

	// add server to circle
	std::vector<int>			AddServer(std::string serverName);

	// fill backward with server name until another server
	void						FillBackward (std::vector<int> poss, std::string serviceName);

	// find closest server
	int							FindForward (int keyPos);

public:

	ConsistingHashing(int size);
	~ConsistingHashing();

	// add server
	int							add_service(std::string serverAddr, int port);

//	// get server by key
//	std::string					get_service(std::string key);

	// print circle
	void						show_ring();

	// get ring
	std::vector<std::string>	get_ring();

	// get service
	std::string					get_service(std::string key);

	// find next server in the ring
	std::string					next_server(std::string key, int hops);
};

#endif /* CONSISTINGHASHING_H_ */
