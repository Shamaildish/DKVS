/*
 * Server.h
 *
 *  Created on: Nov 26, 2020
 *      Author: edan
 */

#ifndef SERVER_H_
#define SERVER_H_

#include "Connection.h"
#include "Hash.h"
#include "Function.h"
#include "Defines"

class Server {
public:
	Server(char* indexerAddr, int indexerPort, int hashSize);
	~Server();

	void run_service(int portPos);

	// get services ports
	std::vector<int> get_services_ports();
	int get_services_ports(int pos);

	// add service
	int add_service (int port);

private:

	std::string				lbAddr;
	std::string				serverAddr;
	int						lbPort;
	int						lbSock;
	std::vector<int>		servicesPorts;
	Hash					hashTable;

	// handlers
	void Server_MessageHandler (Connection* conn, int sock, std::string msg);
	void Server_LBMessageHandler(Connection *conn, int sock, std::string request, int reqType);
	void Server_ClientMessageHandler(Connection *conn, int sock, std::string request, int reqType);

};

#endif /* SERVER_H_ */

