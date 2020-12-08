/////*
//// * main.cpp
//// *
//// *  Created on: Oct 20, 2020
//// *      Author: edan
//// */
//
//
//#include <iostream>
//#include "LoadBalancer.h"
//#include "Server.h"
//#include "Client.h"
//
//int main (int argc, char* argv[])
//{
//	int indexerPort = 8080;
//	int serverPort;
//	int type = atoi (argv[1]);
//
//	switch (type)
//	{
//	case 0:
//	{
//		if (argc != 3)
//		{
//			std::cout << "ERROR: wrong arguments" << std::endl;
//			exit(0);
//		}
//		int hashSize = atoi (argv[2]);
//		std::cout << "	Load Balancer" << std::endl << "	-------------" << std::endl << std::endl;
//		LoadBalancer indexer(indexerPort, hashSize);
//		break;
//	}
//
//	case 1:
//	{
//		if (argc != 4)
//		{
//			std::cout << "ERROR: wrong arguments" << std::endl;
//			exit(0);
//		}
//		int serverPort = atoi (argv[2]);
//
//		std::cout << "	Server" << std::endl << "	------" << std::endl << std::endl;
//		Server server("127.0.0.1", indexerPort, atoi(argv[3]));
//
//		int serviceID = server.add_service(serverPort);
//		server.run_service(serviceID);
//
//		break;
//	}
//	case 2:
//	{
//		std::cout << "	Client" << std::endl << "	------" << std::endl << std::endl;
//		Client client("127.0.0.1", indexerPort);
//	}
//	}
//
//
//
//	return 0;
//}

#include <iostream>
#include "FileHandler.h"

int main()
{
    FileHandler file("testfile.txt");
    std::vector<request*> requests = file.getRequestsFromFile();
    for (int i=0; i<requests.size(); i++)
    {
        std::cout   << "request type: ";
        (requests[i]->GOS == SET) ? std::cout   << "SET" : std::cout   << "GET";
        std::cout   << std::endl << "key: " << requests[i]->key.c_str();
        if (requests[i]->value.size()) std::cout << "value: " << requests[i]->value.c_str() << std::endl;
    }
    return 0;
}