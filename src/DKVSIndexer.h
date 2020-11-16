/*
 * DKVSIndexer.h
 *
 *  Created on: Oct 21, 2020
 *      Author: edan
 */

#include "TCPConnection.h"
#include "ConsistingHashing.h"
#include "Defines"
#include "GeneralFunctions.h"

#ifndef DKVSINDEXER_H_
#define DKVSINDEXER_H_

class DKVS_Indexer {
public:
	DKVS_Indexer(int port, int hashSize);
	~DKVS_Indexer();

	// run indexer
	int run();

private:

	int							indexer_port;
	std::vector<std::string>	servers;	// string <server-address>:<server-port>:<remaining-storage>:<server-capacity>
	ConsistingHashing			hash_ring;
	int							served_sock;

//	// split
//	std::vector<std::string> 	split(std::string msg, char delimiter);

	// set information of the server
	void 						SetInfo(std::string address, int port, int remainingStorage, int capacity, int server);

	// add server
	int 						AddServerToList(std::string address, int port, int capacity);

	// get address and port of the server by server number
	std::string 				Get (int server);

	// find place in servers
	int 						FindAvalibleStorage (int msgSize);

	// store key
	int 						Store(std::string key, std::string serverAddr, int serverPort);

	// get address and port of the server by key
	std::string 				Get(std::string key);

	// messages handlers
	void 						Indexer_MessageHandler (TCPConnection *conn, int sock, std::string msg);
	void 						Indexer_ServerMessageHandler (TCPConnection* conn, int sock, std::string msg, int reqType);
	void 						Indexer_ClientMessageHandler (TCPConnection* conn, int sock, std::string msg, int reqType);
};

#endif /* DKVSINDEXER_H_ */
