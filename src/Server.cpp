/*
 * DKVSServer.cpp
 *
 *  Created on: Oct 21, 2020
 *      Author: edan
 */

#include "Server.h"

void Server_MessageHandler(Connection *conn, int sock, std::string msg);
void Server_LBMessageHandler(Connection *conn, int sock, std::string request, int reqType);
void Server_ClientMessageHandler(Connection *conn, int sock, std::string request, int reqType);

/********************************** CONSTRUCTORS/DESTRUCTOR ***********************************/

Server::Server(char* indexerAddr, int indxPort, int hashSize)
: lbAddr(std::string(indexerAddr)), lbPort(indxPort), servicesPorts(std::vector<int>(0)), lbSock(0), serverAddr(std::string(""))
{
	hashTable.create_table(hashSize);
}

Server::~Server()
{

}


/********************************** PUBLIC ***********************************/

void 				Server::run_service(int portPos)
{

	// get local ip
	Function g;
	serverAddr = g.getIP();

	std::cout	<< "address: " << serverAddr.c_str() << std::endl
				<< "port: " << servicesPorts[portPos] << std::endl;

	if (serverAddr.size() == 0)
	{
		return;
	}

	// listen
	Connection listener;
	listener.listen_on(servicesPorts[portPos]);

	// connect to indexer
	Connection indexerConn;
	char buff[MAX_DATA_SIZE];

	lbSock = indexerConn.connect_to(lbAddr, lbPort);

	// generate hello message for indexer
	/* request = <request-type>~<server-address>:<service-port>:<capacity> */

	sprintf (buff, "%d~%s:%d:%d\0", HELLO, serverAddr.c_str(), servicesPorts[portPos], SERVER_CAPACITY);

	// send hello message to indexer
	indexerConn.send_uni(lbSock, std::string(buff));

	// wait to receive ack from indexer
	if (indexerConn.receive(lbSock, buff, MAX_DATA_SIZE) == 0)
	{
		if (std::string(buff).compare("ACK"))
		{
			std::cout << "ERROR: wrong message from indexer" << std::endl;
			return;
		}
		else
		{
			std::cout << "connected to load balancer at " << lbAddr.c_str() << ":" << lbPort << std::endl;
		}
	}

	else
	{
		std::cout << "ERROR: cannot connect to indexer" << std::endl;
		indexerConn.disconnect(lbSock);
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

		// receive message and send it to message handler
		Server_MessageHandler(&listener, remoteSock, std::string(buff));
	}
}

// get services ports
std::vector<int> 	Server::get_services_ports()
{
	return servicesPorts;
}

int 				Server::get_services_ports(int pos)
{
	return servicesPorts[pos];
}

// set service port
int 				Server::add_service (int port)
{
	servicesPorts.push_back(port);
	return servicesPorts.size() - 1;
}

/********************************** MESSAGE HANDLERS ***********************************/

void 				Server::Server_MessageHandler(Connection* conn, int sock, std::string msg)
{

	Function g;

	// parse request
	/* request = <request-type>~<request> */
	std::vector<std::string> splitted = g.split(msg, '~');
	std::string request = splitted[1];
	int reqType = atoi(splitted[0].c_str());

	if (reqType == REQUEST_GOT || reqType == GOODBYE)
	{
		Server_LBMessageHandler(conn, sock, request, reqType);
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

void 				Server::Server_LBMessageHandler(Connection *conn, int sock, std::string request, int reqType)
{

	Function g;
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

void 				Server::Server_ClientMessageHandler(Connection *conn, int sock, std::string request, int reqType)
{

	Function g;
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

		std::cout << "'" << kv[0] << "' was added successfully to DB" << std::endl;

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

