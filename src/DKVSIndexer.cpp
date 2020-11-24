/*
 * DKVSIndexer.cpp
 *
 *  Created on: Oct 21, 2020
 *      Author: edan
 */

#include "DKVSIndexer.h"

std::string 				Get(int server);
std::string 				Get(std::string key);
void						Indexer_MessageHandler (TCPConnection* conn, int sock, std::string msg);
void 						Indexer_ServerMessageHandler (DKVS_Indexer* indexer, TCPConnection* conn, int sock, std::string msg, int reqType);
void 						Indexer_ClientMessageHandler (DKVS_Indexer* indexer, TCPConnection* conn, int sock, std::string msg, int reqType);

/********************************** CONSTRUCTORS/DESTRUCTOR ***********************************/

DKVS_Indexer::DKVS_Indexer(int port, int hashSize)
: indexer_port(port), servers(std::vector<std::string>(0)), hash_ring(ConsistingHashing(hashSize)), served_sock(0)
//, handler(Indexer_MessageHandler)
{

}

DKVS_Indexer::~DKVS_Indexer()
{

}

/********************************** PUBLIC ***********************************/

// run indexer
int 				DKVS_Indexer::run()
{
	char buff[MAX_DATA_SIZE];
	TCPConnection remoteConnection;
	remoteConnection.listen_on(indexer_port);
	int remoteSock;

	while (1)
	{
		// accept connection
		remoteSock = remoteConnection.accept_connection();

		// receive message
		remoteConnection.receive(remoteSock, buff, MAX_DATA_SIZE);

		// call message handler
		Indexer_MessageHandler(&remoteConnection, remoteSock, std::string(buff));
	}

}

/********************************** PRIVATE ***********************************/


// get address and port of the server by server number
std::string 		DKVS_Indexer::Get (int server)
{
	GeneralFunctions g;
	std::vector<std::string> splitted = g.split(servers[server], ':');
	return std::string (splitted[0] + ":" + splitted[1]);
}


/********************************** CLIENT REQUEST ***********************************/

// get address
std::string 		DKVS_Indexer::Get(std::string key)
{
	return  hash_ring.get_service(key);
}

/********************************** HANDLERS ***********************************/

void 				DKVS_Indexer::Indexer_MessageHandler (TCPConnection* conn, int sock, std::string msg)
{

	/* msg = <request-type>~<request> */

	GeneralFunctions g;
	std::vector<std::string> splitted = g.split(msg, '~');

	int reqType = atoi((splitted[0]).c_str());
	std::string request = splitted[1];

	// message from server
	if (reqType == HELLO || reqType == GOODBYE || reqType == RESPOND_GOT)
	{
		Indexer_ServerMessageHandler (conn, sock, request, reqType);
	}


	else if (reqType == SET || reqType == GET)
		// message from client
	{
		Indexer_ClientMessageHandler (conn, sock, request, reqType);
	}

	// wrong message type
	else
	{
		std::cout << "ERROR: wrong type of message" << std::endl;
		conn->disconnect(sock);
	}

}

void 				DKVS_Indexer::Indexer_ServerMessageHandler (TCPConnection* conn, int sock, std::string request, int requestType)
{

	// parse message
	GeneralFunctions g;
	std::vector<std::string> splitted = g.split(request, ':');

	switch (requestType)
	{

	/* request = <server-address>:<service-port>:<capacity> */
	case HELLO:
	{

		// add the server to the list
		hash_ring.add_service(splitted[0], atoi(splitted[1].c_str()));
		servers.push_back(request);

		// send ack
		conn->send_uni(sock, std::string("ACK"));

		if (DEBUG)
		{
			std::cout << "new connection " << splitted[0].c_str() << ":" << splitted[1].c_str() << std::endl;
		}

		break;
	}

	/* request = <server-address>:<server-port> */
	case GOODBYE:
	{
		// TODO: build a removing mechanism
		break;
	}

	/* request = <server-address>:<server-port>:<client-sock>:<key>:<br-counter>:<got> */
	case RESPOND_GOT:
	{

		if (DEBUG)
		{
			std::cout << "RESPOND_GOT " << request.c_str() << std::endl;
		}

		char* resStr;
		char* reqStr;

		std::string 		got(splitted[splitted.size() - 1]);		// get got

		std::string 		serverInfo(splitted[0]);				// get server info
		serverInfo.append(":").append(splitted[2]);

		int 				brCounter = atoi(splitted[4].c_str());	// get break rule counter

		std::string 		key = splitted[3];						// get key

		int clientSock = atoi(splitted[2].c_str());					// get socket number of the client

		switch (atoi(got.c_str()))
		{

		// server does not hold the value
		case 0:
		{

			// breaking rule counter
			if (brCounter == servers.size())
			{
				// no such key in servers
				sprintf (resStr, "%d:0.0.0.0\0", GET);
				conn->send_uni(clientSock, std::string(resStr));
			}
			else
			{
				brCounter++;
			}

			// find next server in ring
			serverInfo = hash_ring.next_server(key, brCounter);

			// generate new request
			/* request = <request-type>~<server-address>:<server-port>:<client-sock>:<key>:<br-counter> */
			sprintf (reqStr, "%d~%s:%d:%s:%d\0", REQUEST_GOT, serverInfo.c_str(), clientSock, key.c_str(), brCounter);

			// connect to server
			TCPConnection serverConn;
			std::vector<std::string> srvInf = g.split (serverInfo, ':');
			int serverSock = serverConn.connect_to(srvInf[0], atoi(srvInf[1].c_str()));

			// send request to server
			serverConn.send_uni(serverSock, std::string(reqStr));
			break;
		}

		// server is holding the value
		case 1:
		{
			// generate respond
			/* respond = <request-type>~<server-address>:<server-port> */
			sprintf (resStr, "%d~%s\0", GET, serverInfo.c_str());

			// send respond
			conn->send_uni(clientSock, std::string(resStr));

			break;
		}

		// wrong answer
		default:
		{
			std::cout << "ERROR: wrong message from server" << std::endl;
			conn->disconnect(sock);
			return;
		}
		}

	}
	}

}

void 				DKVS_Indexer::Indexer_ClientMessageHandler (TCPConnection *conn, int sock, std::string key, int requestType)
{

	// no running servers
	if (!servers.size())
	{
		std::cout << "No running services" << std::endl;
		return;
	}

	GeneralFunctions g;
	char* 						charReqType;
	sprintf (charReqType, "%d~\0", requestType);

	std::string 				respond(charReqType);
	std::string 				serverInfo;

	// get server info
	serverInfo = Get(key);

	// generate respond
	respond.append(serverInfo);

	// return address to client
	if (requestType == SET)
	{
		conn->send_uni(sock, respond);
	}

	// start searching for key at servers
	else
	{
		/* request = <request-type>~<server-address>:<server-port>:<client-sock>:<key>:<br-counter> */
		// generate request to server
		char* gotMsg;
		sprintf (gotMsg, "%d~%s:%d:%s:0\0", REQUEST_GOT, serverInfo.c_str(), sock, key.c_str());

		// connect to server
		TCPConnection serverConn;
		std::vector<std::string> serverInf = g.split (serverInfo, ':');
		int serverSock = serverConn.connect_to(serverInf[0], atoi(serverInf[1].c_str()));

		// send request to server
		serverConn.send_uni(serverSock, std::string(gotMsg));
	}
}

