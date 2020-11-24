/*
 * DKVSServer.h
 *
 *  Created on: Oct 21, 2020
 *      Author: edan
 */

#ifndef DKVSSERVER_H_
#define DKVSSERVER_H_


#include "TCPConnection.h"
#include "HashTable.h"
#include "GeneralFunctions.h"
#include "Defines"

class DKVS_Server{

public:
	DKVS_Server(char* indexerAddr, int indexerPort, int hashSize);
	~DKVS_Server();

	void run_service(int portPos);

	// get services ports
	std::vector<int> get_services_ports();
	int get_services_ports(int pos);

	// add service
	int add_service (int port);

private:

	std::string				indexerAddr;
	std::string				serverAddr;
	int						indexerPort;
	int						indexerSock;
	std::vector<int>		servicesPorts;
	HashTable				hashTable;

	// handlers
	void Server_MessageHandler (TCPConnection* conn, int sock, std::string msg);
	void Server_IndexerMessageHandler(TCPConnection *conn, int sock, std::string request, int reqType);
	void Server_ClientMessageHandler(TCPConnection *conn, int sock, std::string request, int reqType);

};

#endif /* DKVSSERVER_H_ */
