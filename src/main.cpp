///*
// * main.cpp
// *
// *  Created on: Oct 20, 2020
// *      Author: edan
// */


#include <iostream>
#include "LoadBalancer.h"
#include "Server.h"
#include "Client.h"

int main (int argc, char* argv[])
{
	int indexerPort = 8080;
	int serverPort;
	int type = atoi (argv[1]);

	switch (type)
	{
	case 0:
	{
		if (argc != 3)
		{
			std::cout << "ERROR: wrong arguments" << std::endl;
			exit(0);
		}
		int hashSize = atoi (argv[2]);
		std::cout << "	Load Balancer" << std::endl << "	-------------" << std::endl << std::endl;
		LoadBalancer indexer(indexerPort, hashSize);
		break;
	}

	case 1:
	{
		if (argc != 4)
		{
			std::cout << "ERROR: wrong arguments" << std::endl;
			exit(0);
		}
		int serverPort = atoi (argv[2]);

		std::cout << "	Server" << std::endl << "	------" << std::endl << std::endl;
		Server server("127.0.0.1", indexerPort, atoi(argv[3]));

		int serviceID = server.add_service(serverPort);
		server.run_service(serviceID);

		break;
	}
	case 2:
	{
		std::cout << "	Client" << std::endl << "	------" << std::endl << std::endl;
		Client client("127.0.0.1", indexerPort);
	}
	}



	return 0;
}
