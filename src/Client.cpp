/*
 * DKVSClient.cpp
 *
 *  Created on: Nov 8, 2020
 *      Author: edan
 */

#include "Client.h"

Client::Client(std::string lbAddr, int lbPort)
: lb_address(lbAddr), lb_port (lbPort), lb_sock(0)
{
	Run();
}

Client::~Client()
{

}

/*					PRIVATE					*/

// menu
void 			Client::Menu()
{

	std::cout	<< std::endl
			<< "		MENU" << std::endl
			<< "		----" << std::endl
			<< std::endl
			<< "	-------------------------" << std::endl
			<< "	| enter 's' to set	|" << std::endl
			<< "	| enter 'g' to get	|" << std::endl
			<< "	| enter 'q' to quite    |" << std::endl
			<< "	-------------------------" << std::endl
			<< std::endl;
}

// run
void 			Client::Run()
{

	Function g;
	char buff[MAX_DATA_SIZE];
	std::string key;
	std::string value;
	std::string respond("");
	char usrInput;

	// connect to load balancer
	Connection lbConn;


	// while user not quieting
	while (1)
	{
		lb_sock = lbConn.connect_to(lb_address, lb_port);
		// show menu
		Menu();

		// get user input
		std::cin >> usrInput;

		// quieting
		if (usrInput == 'q')
		{
			std::cout << std::endl << "	Bye Bye =]" << std::endl;
			return;
		}

		// get key
		std::cout << "Enter key: ";
		std::cin >> key;

		if (DEBUG)
		{
			std::cout << "key: " << key.c_str() << std::endl;
		}

		// generate message to indexer
		/* request = <request-type>~<key> */
		sprintf (buff, "%d~%s\0", SET, key.c_str());

		// send message to indexer
		lbConn.send_uni(lb_sock, std::string(buff));

		// wait for indexer respond
		memset(buff, '\0', MAX_DATA_SIZE);
		lbConn.receive(lb_sock, buff, MAX_DATA_SIZE);

		if (atoi((char*)&buff[0]) != SET)
		{
			std::cout << "ERROR: wrong message type from lb" << std::endl;
			exit(0);
		}

		// get server info
		/* buff = <request-type>~<server-address>:<server-port> */
		std::vector<std::string> splitted = g.split(std::string(buff).substr(2,std::string(buff).size()), ':');
		std::string serverAddr = splitted[0];
		int serverPort = atoi(splitted[1].c_str());

		// connect to server
		Connection serverConn;
		int serverSock = serverConn.connect_to(serverAddr, serverPort);

		// set
		if (usrInput == 's')
		{

			// get value
			std::cout << "Enter value: ";
			std::cin >> value;

			// generate request to server
			memset(buff, '\0', MAX_DATA_SIZE);
			sprintf (buff, "%d~%s:%s\0", SET, key.c_str(), value.c_str());

			// send request to server
			serverConn.send_uni(serverSock, std::string(buff));

			// wait for server respond
			memset (buff, '\0', MAX_DATA_SIZE);
			serverConn.receive(serverSock, buff, MAX_DATA_SIZE);

			// parse respond
			/* buff = <request-type>~<key>:<ack> */
			std::vector<std::string> serverRes = g.split(std::string(buff), '~');
			std::vector<std::string> respond = g.split(serverRes[1], ':');
			if (atoi(serverRes[0].c_str()) != SET || std::string(key).compare(respond[0]) || respond[1].compare("ACK"))
			{
				std::cout << "ERROR: server did not added the key to db" << std::endl;
				continue;
			}
			std::cout << key.c_str() << " was added to " << serverAddr.c_str() << ":" << serverPort << std::endl;
		}

		else if (usrInput == 'g')
		{
			// generate request to server
			memset (buff, '\0', MAX_DATA_SIZE);
			sprintf (buff, "%d~%s\0", GET, key.c_str());

			// send request to server
			serverConn.send_uni(serverSock, std::string(buff));

			// wait for server respond
			memset (buff, '\0', MAX_DATA_SIZE);
			serverConn.receive(serverSock, buff, MAX_DATA_SIZE);

			// parse respond
			/* buff = <request-type>~<key>:<value> */
			std::vector<std::string> serverRes = g.split(std::string(buff), '~');
			std::vector<std::string> respond = g.split(serverRes[1], ':');
			if (atoi(serverRes[0].c_str()) != GET || std::string(key).compare(respond[0]))
			{
				std::cout << "ERROR: server does not hold the key" << std::endl;
				return;
			}
			std::cout << "the value of key '" << key << "' is '" << respond[1] << "'" << std::endl;
		}

		else
		{
			// wrong input
			std::cout << std::endl << "wrong input, try again" << std::endl << std::endl;
		}

		lbConn.disconnect(lb_sock);

	}
}

