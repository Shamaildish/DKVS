///*
// * main.cpp
// *
// *  Created on: Oct 20, 2020
// *      Author: edan
// */
//
//
//#include <iostream>
//#include "TCPConnection.h"
//
//void Client_MessageReceived (TCPConnection* conn, int sock, std::string msg);
//void Server_MessageReceived (TCPConnection* conn, int sock, std::string msg);
//
//void Usage()
//{
//	if (DEBUG)
//	{
//		std::cout << "Usage()" << std::endl;
//	}
//}
//
//int main (int argc, char* argv[])
//{
//
//	std::string msg;
//	char buff[1024];
//	int side = atoi(argv[1]);
//
//
//	switch(side)
//	{
//	case 0:
//	{
//		if (DEBUG)
//		{
//			std::cout << " Client" << std::endl << std::endl;
//		}
//		if (argc != 4)
//		{
//			Usage();
//			exit(EXIT_SUCCESS);
//		}
//
//		msg = std::string("MSG from Client");
//
//		TCPConnection client (Client_MessageReceived);
//		int serverSock = client.connect_to(std::string(argv[2]), atoi(argv[3]));
//
//		std::cout << "sending '" << msg.c_str() << "'" << std::endl;
//		client.send_uni(serverSock, msg);
//
//		client.receive(serverSock, buff, 1024);
//		Client_MessageReceived(&client, serverSock, std::string(buff));
//
//		break;
//	}
//	case 1:
//	{
//		if (DEBUG)
//		{
//			std::cout << " Server" << std::endl << std::endl;
//		}
//		if (argc != 3)
//		{
//			Usage();
//			exit(EXIT_SUCCESS);
//		}
//
//		TCPConnection server (Server_MessageReceived);
//
//		server.listen_on(atoi(argv[2]));
//
//		int clientSock = server.accept_connection();
//
//		server.receive(clientSock, buff, 1024);
//		Server_MessageReceived(&server, clientSock, std::string(buff));
//
//		break;
//	}
//	default:
//		Usage();
//	}
//	return 0;
//}
//
//void Client_MessageReceived (TCPConnection* conn, int sock, std::string msg)
//{
//	if (DEBUG)
//	{
//		std::cout << " Client_MessageReceived" << std::endl;
//	}
//	std::cout << "Server> " << msg.c_str() << std::endl;
//}
//
//void Server_MessageReceived (TCPConnection* conn, int clisetSock, std::string msg)
//{
//	std::string respondMsg("MSG from Server");
//	if (DEBUG)
//	{
//		std::cout << " Server_MessageReceived" << std::endl;
//	}
//	std::cout << "Client> " << msg.c_str() << std::endl;
//	conn->send_uni(clisetSock, respondMsg);
//}



#include <iostream>
#include "DKVSIndexer.h"
#include "DKVSServer.h"
#include "DKVSClient.h"

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
		DKVS_Indexer indexer(indexerPort, hashSize);
		indexer.run();
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
		DKVS_Server server("127.0.0.1", indexerPort, atoi(argv[3]));

		int serviceID = server.add_service(serverPort);
		server.run_service(serviceID);

		break;
	}
	case 2:
	{
		std::cout << "	Client" << std::endl << "	------" << std::endl << std::endl;
		DKVS_Client client("127.0.0.1", indexerPort);
	}
	}


	//	TCPConnection conn;
	//	char buff[100];
	//	switch (atoi (argv[1]))
	//	{
	//	case 0:
	//	{
	//
	//
	//		conn.listen_on(4040);
	//		int conSock = conn.accept_connection();
	//		conn.receive(conSock, buff, 100);
	//		break;
	//	}
	//
	//	case 1:
	//	{
	//		int conSock = conn.connect_to(std::string("127.0.0.1"), 4040);
	//		conn.send_uni(conSock, std::string("BUFF"));
	//		conn.receive(conSock, buff, 100);
	//		break;
	//	}
	//	}


	return 0;
}
