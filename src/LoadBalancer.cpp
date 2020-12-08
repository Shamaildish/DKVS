/*
 * DKVSIndexer.cpp
 *
 *  Created on: Oct 21, 2020
 *      Author: edan
 */

#include "LoadBalancer.h"

std::string 				Get(int server);
std::string 				Get(std::string key);
void						LB_MessageHandler (Connection* conn, int sock, std::string msg);
void 						LB_ServerMessageHandler (LoadBalancer* indexer, Connection* conn, int sock, std::string msg, int reqType);
void 						LB_ClientMessageHandler (LoadBalancer* indexer, Connection* conn, int sock, std::string msg, int reqType);

/********************************** CONSTRUCTORS/DESTRUCTOR ***********************************/

LoadBalancer::LoadBalancer(int port, int hashSize)
: lb_port(port), servers(std::vector<std::string>(0)), hash_ring(ConssistingHash(hashSize))
//, conn(new Connection()), temp_sock(-1), temp_msg(std::string(0))
{
    connection = nullptr;
    temp_sock = -1;
    temp_msg = std::string("");
    run();
}

LoadBalancer::LoadBalancer(LoadBalancer *lb)
        : lb_port(lb->lb_port), servers(lb->servers), hash_ring(lb->hash_ring)
//        , conn(lb->conn), temp_sock(lb->temp_sock), temp_msg(lb->temp_msg)
{
    connection = lb->getConnection();
    temp_sock = lb->getSock();
    temp_msg = lb->getMessage();
}

LoadBalancer::~LoadBalancer()
{

}

/********************************** PUBLIC ***********************************/

void LoadBalancer::setCI(int sock, std::string message)
{
    temp_sock = sock;
    temp_msg = message;
}

int LoadBalancer::getSock()
{
    return temp_sock;
}

std::string LoadBalancer::getMessage()
{
    return temp_msg;
}

Connection* LoadBalancer::getConnection()
{
    return connection;
}

void    ThreadFunc(LoadBalancer *lb)
{
    lb->LB_MessageHandler(lb->getConnection(), lb->getSock(), lb->getMessage());
}

// run load balancer
int 				LoadBalancer::run()
{
    std::thread threads[MAX_THREADS];
    int threadCount = 0;
	char buff[MAX_DATA_SIZE];
	Connection remoteConnection;
	remoteConnection.listen_on(lb_port);
	connection = &remoteConnection;
	int remoteSock;

	while (threadCount <= MAX_THREADS)
	{
		// accept connection
		std::cout << "waiting for new connection..." << std::endl;
        remoteSock = remoteConnection.accept_connection();
        std::cout << "new connection!" << std::endl;
        // receive message
		remoteConnection.receive(remoteSock, buff, MAX_DATA_SIZE);

		setCI(remoteSock, std::string(buff));

        // call thread function
        threads[threadCount] = std::thread(ThreadFunc, this);
        threadCount++;
	}

	for (int thrd=0; thrd<threadCount; thrd++)
    {
        threads[thrd].join();
    }

}


/********************************** CLIENT REQUEST ***********************************/

// get address
std::string 		LoadBalancer::GetPos(std::string key)
{
	return  hash_ring.get_service(key);
}

/********************************** HANDLERS ***********************************/

void 				LoadBalancer::LB_MessageHandler (Connection* conn, int sock, std::string msg)
{

	/* msg = <request-type>~<request> */

	Function g;

	std::vector<std::string> splitted = g.split(msg, '~');

	int reqType = atoi((splitted[0]).c_str());
	std::string request = splitted[1];

	std::cout << "msg --- " << msg.c_str() << std::endl;

	// message from server
	if (reqType == HELLO || reqType == GOODBYE || reqType == RESPOND_GOT)
	{
		LB_ServerMessageHandler (conn, sock, request, reqType);
	}

	else if (reqType == SET || reqType == GET)
		// message from client
	{
		LB_ClientMessageHandler (conn, sock, request, reqType);
	}

	else if (reqType == IS_FILE)
    {
	    std::cout << "request for file" << std::endl;
    }


	// wrong message type
	else
	{
		std::cout << "ERROR: wrong type of message" << std::endl;
		conn->disconnect(sock);
	}

}

void 				LoadBalancer::LB_ServerMessageHandler (Connection* conn, int sock, std::string request, int requestType)
{

	// parse message
	Function g;
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
			Connection serverConn;
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

			if (DEBUG)
			{
				std::cout << serverInfo.c_str() << std::endl;
			}
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

void 				LoadBalancer::LB_ClientMessageHandler (Connection *conn, int sock, std::string key, int requestType)
{
	if (DEBUG)
	{
		std::cout << "Client request '" << key.c_str() << "', will be served by ";
	}

	// no running servers
	if (!servers.size())
	{
		std::cout << "No running services" << std::endl;
		return;
	}

	Function g;
	char* 						charReqType;
	sprintf (charReqType, "%d~\0", requestType);

	std::string 				respond(charReqType);
	std::string 				serverInfo;

	// get server info
	serverInfo = GetPos(key);

	// generate respond
	respond.append(serverInfo);

	// return address to client
	if (requestType == SET)
	{
		if (DEBUG)
		{
			std::cout << serverInfo.c_str() << std::endl;
		}
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
		Connection serverConn;
		std::vector<std::string> serverInf = g.split (serverInfo, ':');
		int serverSock = serverConn.connect_to(serverInf[0], atoi(serverInf[1].c_str()));

		// send request to server
		serverConn.send_uni(serverSock, std::string(gotMsg));
	}

	conn->disconnect(sock);
}





