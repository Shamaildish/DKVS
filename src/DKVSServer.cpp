/*
 * DKVSServer.cpp
 *
 *  Created on: Oct 21, 2020
 *      Author: edan
 */

#include "DKVSServer.h"

void Server_MessageHandler(TCPConnection *conn, int sock, std::string msg);
void Server_IndexerMessageHandler(TCPConnection *conn, int sock, std::string request, int reqType);
void Server_ClientMessageHandler(TCPConnection *conn, int sock, std::string request, int reqType);
int ConnectToIndexer (TCPConnection* indexerConn);

/********************************** CONSTRUCTORS/DESTRUCTOR ***********************************/

DKVS_Server::DKVS_Server(char* indexerAddr, int indxPort, int hashSize)
: indexerAddr(std::string(indexerAddr)), indexerPort(indxPort), servicesPorts(std::vector<int>(0)), indexerSock(0), serverAddr(std::string(""))
{
	hashTable.create_table(hashSize);
}

DKVS_Server::~DKVS_Server()
{

}


/********************************** PUBLIC ***********************************/

void 				DKVS_Server::run_service(int portPos)
{

	// get local ip
	GeneralFunctions g;
	serverAddr = g.getIP();
	if (serverAddr.size() == 0)
	{
		return;
	}

	// listen
	TCPConnection listener;
	listener.listen_on(servicesPorts[portPos]);

	// connect to indexer
	TCPConnection indexerConn;
	char buff[MAX_DATA_SIZE];

	indexerSock = indexerConn.connect_to(indexerAddr, indexerPort);

	// generate hello message fot indexer
	/* request = <request-type>~<server-address>:<service-port>:<capacity> */

	sprintf (buff, "%d~%s:%d:%d\0", HELLO, serverAddr.c_str(), servicesPorts[portPos], SERVER_CAPACITY);

	// send hello message to indexer
	indexerConn.send_uni(indexerSock, std::string(buff));

	// wait to receive ack from indexer
	if (indexerConn.receive(indexerSock, buff, MAX_DATA_SIZE) == 0)
	{
		if (std::string(buff).compare("ACK"))
		{
			std::cout << "ERROR: wrong message from indexer" << std::endl;
			return;
		}
		else
		{
			std::cout << "connected to load balancer at " << indexerAddr.c_str() << ":" << indexerPort << std::endl;
		}
	}

	else
	{
		std::cout << "ERROR: cannot connect to indexer" << std::endl;
		indexerConn.disconnect(indexerSock);
		return;
	}


	// start routine
	int remoteSock;
	while (1)
	{
		// reset buff
		memset (buff, '\0', MAX_DATA_SIZE);

		// accept connection
		remoteSock = listener.accept_connection();

		// receive message
		listener.receive(remoteSock, buff, MAX_DATA_SIZE);

		if (DEBUG)
		{
			std::cout << buff << std::endl;
		}

		// receive message and send it to message handler
		Server_MessageHandler(&listener, remoteSock, std::string(buff));
	}
}

// get services ports
std::vector<int> 	DKVS_Server::get_services_ports()
{
	return servicesPorts;
}

int 				DKVS_Server::get_services_ports(int pos)
{
	return servicesPorts[pos];
}

// set service port
int 				DKVS_Server::add_service (int port)
{
	servicesPorts.push_back(port);
	return servicesPorts.size() - 1;
}

/********************************** PRIVATE ***********************************/

int 				DKVS_Server::ConnectToIndexer (TCPConnection* indexerConn)
{
	indexerSock = indexerConn->connect_to(indexerAddr, indexerPort);

	if (DEBUG)
	{
		std::cout << "ConnectToIndexer" << std::endl;
	}

	char buff[MAX_DATA_SIZE];
	// generate hello message
	/* request = <request-type>~<server-address>:<service-port>:<capacity> */
	sprintf (buff, "%d~%s:%d:%d\0", HELLO, indexerAddr.c_str(), indexerPort, SERVER_CAPACITY);

	// send hello message to indexer
	indexerConn->send_uni(indexerSock, std::string(buff));

	std::cout << "start " << buff << std::endl;

	// wait to receive ack from indexer
	if (indexerConn->receive(indexerSock, buff, MAX_DATA_SIZE))
	{
		return 1;
	}
	else
	{
		std::cout << " end " << buff << std::endl;
		if (std::string(buff).compare("ACK"))
		{
			std::cout << "ERROR: wrong message from indexer" << std::endl;
			return 1;
		}
	}
	return 0;
}

/********************************** MESSAGE HANDLERS ***********************************/

void 				DKVS_Server::Server_MessageHandler(TCPConnection* conn, int sock, std::string msg)
{

	if (DEBUG)
	{
		std::cout << "Server_MessageHanlder" << std::endl << msg.c_str() << std::endl;
	}

	// parse request
	/* request = <request-type>~<request> */


	GeneralFunctions g;
	std::vector<std::string> splitted = g.split(msg, '~');
	std::string request = splitted[1];
	int reqType = atoi(splitted[0].c_str());
	if (DEBUG)
	{
		std::cout << reqType << " " << request.c_str() << std::endl;
	}
	if (reqType == REQUEST_GOT || reqType == GOODBYE)
	{
		Server_IndexerMessageHandler(conn, sock, request, reqType);
	}

	else if (reqType == SET || reqType == GET)
	{
		Server_ClientMessageHandler(conn, sock, request, reqType);
	}

	else
	{
		std::cout << "ERROR: wrong request type" << std::endl;
		conn->disconnect(sock);
	}
}

void 				DKVS_Server::Server_IndexerMessageHandler(TCPConnection *conn, int sock, std::string request, int reqType)
{
	if (DEBUG)
	{
		std::cout << "Server_IndexerMessageHandler" << std::endl;
	}

	GeneralFunctions g;
	std::vector<std::string> splitted = g.split(request, ':');
	std::string key = splitted[0];
	std::string respond("");
	char* respondChar;
	switch (reqType)
	{

	case REQUEST_GOT:
	{
		/* request = <request-type>~<server-address>:<server-port>:<client-sock>:<key>:<lb-counter>:<got> */

		// check if got key
		int exist;

		// is not exist
		hashTable.is_exist(key) ? exist = 1: exist = 0;

		// generate respond to lb
		sprintf (respondChar, "%d~%s:%s:%s:%s:%s:%d", RESPOND_GOT, splitted[0].c_str(), splitted[1].c_str(), splitted[2].c_str(), splitted[3].c_str(), splitted[4].c_str(), exist);

		// send respond to lb
		conn->send_uni(sock, std::string(respondChar));
		break;
	}

	case GOODBYE:
	{
		break;
	}

	default:
	{
		std::cout << "ERROR: wrong message type" << std::endl;
		return;
	}
	}
}

void 				DKVS_Server::Server_ClientMessageHandler(TCPConnection *conn, int sock, std::string request, int reqType)
{
	if (DEBUG)
	{
		std::cout << "Server_ClientMessageHandler" << std::endl;
	}

	GeneralFunctions g;
	char resStr[int(MAX_DATA_SIZE/10)];

	// split
	std::vector<std::string> kv = g.split (request, ':');

	// generate base respond
	sprintf (resStr, "%d~%s:", reqType, kv[0].c_str());
	std::string respond(resStr);

	switch (reqType)
	{

	case SET:
		/* request = <key>:<value> */
	{
		// store key-value
		hashTable.insert(kv[0], kv[1]);

		// return ack to client
		respond.append("ACK");

		break;
	}

	case GET:
		/* request = <key> */
	{
		// get value
		std::string value = hashTable.get(kv[0]);

		// check value validation
		if (value.size() == 0)
		{
			std::cout << "strange... server is not holding the key" << std::endl;

			// generate respond to client
			respond.append("0.0.0.0:0");

			// send respond
			conn->send_uni(sock, std::string(respond));
			return;
		}

		// generate respond
		respond.append(value);

		break;
	}

	default:
	{
		std::cout << "wrong message type from client" << std::endl;
		return;
	}
	}

	// send respond to client
	conn->send_uni(sock, respond);
}


