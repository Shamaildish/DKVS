/*
 * ConssistingHash.cpp
 *
 *  Created on: Oct 24, 2020
 *      Author: edan
 */

#include "ConssistingHash.h"


/*								CONSTRUCTOR/DESTRUCTOR								*/

ConssistingHash::ConssistingHash(int circleSize)
: ring(std::vector<std::string>(circleSize)), size(circleSize), occ_spots(0)
{
	std::fill (ring.begin(), ring.end(), std::string("0.0.0.0:0000"));
}

ConssistingHash::~ConssistingHash()
{

}

/*								PRIVATE								*/

// hash function for full server names (including the virtual/original bit)
int							ConssistingHash::HashName(std::string serverLongName)
{
	if (((int)((uint64_t)(atoi(serverLongName.c_str()) * HASH_DOUBLE_VALUE)) % size) < 0)
	{
		return -1*(((int)((uint64_t)(atoi(serverLongName.c_str()) * HASH_DOUBLE_VALUE)) % size));
	}
	return ((int)((uint64_t)(atoi(serverLongName.c_str()) * HASH_DOUBLE_VALUE)) % size);
}

// generate server's name
void				 		ConssistingHash::GenerateServerName(std::string serverInfo, std::vector<int> *names)
{

	int oct1, oct2, oct3, oct4, port, type;
	sscanf (serverInfo.c_str(), "%d.%d.%d.%d:%d:", &oct1, &oct2, &oct3, &oct4, &port);

	char serverLongStr[32];

	if (DEBUG)
	{
		std::cout << "virtual names: ";
	}
	// generate server short name
	for (int i=0; i<=VIRTUAL_NAMES; i++)
	{
		sprintf (serverLongStr, "%d%d%d%d%d%d\0",oct1, oct2, oct3, oct4, port, i);
		(*names)[i] = HashName(std::string(serverLongStr));

		if (DEBUG)
		{
			std::cout << (*names)[i] << " ";
		}

	}

	if (DEBUG)
	{
		std::cout << std::endl;
	}

}

// add server to circle
std::vector<int>			ConssistingHash::AddServer(std::string m_ServerInfo)
{

	std::string serverInfo = m_ServerInfo;
	serverInfo.append(":1");
	std::vector<int> serverNames(VIRTUAL_NAMES + 1);
	std::string	curr;
	GenerateServerName(serverInfo, &serverNames);
	for (int i=0; i<=VIRTUAL_NAMES; i++)
	{
		curr = ring[serverNames[i]];

		// server name with additional int for indication virtual/original name
		if (*(curr.substr(curr.size()-1, 1).c_str()) == '0')
		{
			ring[serverNames[i]] = serverInfo;
		}
		else
		{
			std::cout << "ERROR: collision in hash" << std::endl;
			return std::vector<int>(VIRTUAL_NAMES+1);
		}
	}

	// server original location
	return serverNames;

}

// fill backward with server name until another server
void						ConssistingHash::FillBackward (std::vector<int> poss, std::string m_ServerInfo)
{

	std::string serverInfo = m_ServerInfo;
	serverInfo.append(":0");

	int pos;
	int hops = 0;

	// each name
	for (int vn=0; vn<=VIRTUAL_NAMES; vn++)
	{

		pos = poss[vn];

		// going backward
		while (hops < size)
		{
			pos --;

			if (pos == -1)
			{
				pos += size;
			}

			// if current position on circle has a virtual name
			if (atoi((const char*)&(ring[pos].at(ring[pos].size()-1))) == 1)
			{
				break;
			}

			ring[pos] = serverInfo;
			hops++;
		}
	}

	if (DEBUG)
	{
		std::cout << hops << " places were filled in the circle" << std::endl;
	}
}

// find closest server
int							ConssistingHash::FindForward (int keyPos)
{

	// get server at key value position
	std::string fullServerInfo = ring[keyPos];

	// drop last char (indication of virtual name)
	std::string serverInfo = fullServerInfo.substr(0, fullServerInfo.size() - 2);

	// get server virtual names
	std::vector<int> poss(VIRTUAL_NAMES + 1);

	GenerateServerName(serverInfo, &poss);

	// pick the next position clockwise
	int closestServer = INFINITY;
	for (int vn=0; vn<=VIRTUAL_NAMES; vn++)
	{
		// find the smallest positive distance (clockwise) of virtual name
		if (((poss[vn] - keyPos) > 0) && ((poss[vn] - keyPos) < (closestServer - keyPos)))
		{
			closestServer = poss[vn];
		}
	}

	/* if there is no positive distance (when keyPos is bigger then all server virtual names)
	   pick the smallest position */
	if (closestServer == INFINITY)
	{
		for (int vn=0; vn<=VIRTUAL_NAMES; vn++)
		{
			if (poss[vn] < closestServer)
			{
				closestServer = poss[vn];
			}
		}
	}

	return closestServer;
}

/*								PUBLIC								*/

int							ConssistingHash::add_service(std::string addr, int port)
{
	if (DEBUG)
	{
		std::cout << std::endl << "Add Service" << std::endl << "-----------" << std::endl;
	}

	std::string serverInfo(addr);
	char portNum[10];
	sprintf (portNum, ":%d\0", port);

	serverInfo.append(std::string(portNum));

	if (DEBUG)
	{
		std::cout << "service info: " << serverInfo.c_str() << std::endl;
	}

	occ_spots ++;
	std::vector<int> serverVNames;
	serverVNames = AddServer(serverInfo);
	FillBackward(serverVNames, serverInfo);
	return 0;
}

int							ConssistingHash::HashKey(std::string key)
{
	double res;
	for (int i=0; i<key.size(); i++)
	{
		res += int(key.at(i));
	}
	res *= 3146.14821;
	return int(res) % ring.size();
}

void 						ConssistingHash::show_ring()
{
	if (DEBUG)
	{
		std::cout << "get_circle" << std::endl;
	}

	for (int i=0; i<size; i++)
	{
		std::cout << i << ": " << ring[i].c_str() << std::endl;
	}
}

std::vector<std::string>	ConssistingHash::get_ring()
{
	return ring;
}

std::string					ConssistingHash::get_service(std::string key)
{
	int keyPos = HashKey(key);
	return ring[keyPos].substr(0, ring[keyPos].size() - 2);
}

std::string					ConssistingHash::next_server(std::string key, int hops)
{
	int prevKeyPos = HashKey(key);
	int keyPos = prevKeyPos;
	for (int i=0; i<hops; i++)
	{
		keyPos = FindForward(prevKeyPos);

		// if same server
		if (ring[prevKeyPos].compare(0, ring[prevKeyPos].size() - 2, ring[keyPos].substr(0, ring[keyPos].size() - 2)) == 0)
		{
			i--;
		}
		prevKeyPos = keyPos + 1;

	}
	return ring[keyPos].substr(0, ring[keyPos].size());
}
