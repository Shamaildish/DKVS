/*
 * LoadBalancer.h
 *
 *  Created on: Nov 26, 2020
 *      Author: edan
 */

#ifndef LOADBALANCER_H_
#define LOADBALANCER_H_

#include "Connection.h"
#include "ConssistingHash.h"
#include "Defines"
#include "Function.h"
#include "FileHandler.h"
#include <thread>

class LoadBalancer {
public:
	LoadBalancer(int port, int hashSize);
	LoadBalancer(LoadBalancer *lb);
	~LoadBalancer();

    void                        setCI(int sock, std::string message);
    Connection*                 getConnection();
    int                         getSock();
    std::string                 getMessage();

    void 				        LB_MessageHandler (Connection *conn, int sock, std::string msg);


private:

	int							lb_port;
	std::vector<std::string>	servers;	// string <server-address>:<server-port>:<remaining-storage>:<server-capacity>
	ConssistingHash				hash_ring;
	Connection*                 connection;
	int                         temp_sock;
	std::string                 temp_msg;

//	Connection*                 conn;
//	int                         temp_sock;
//	std::string                 temp_msg;

    // run indexer
    int                         run();

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
//    void 				LB_MessageHandler (Connection *conn, int sock, std::string msg);
	void 				LB_ServerMessageHandler (Connection* conn, int sock, std::string msg, int reqType);
	void 				LB_ClientMessageHandler (Connection* conn, int sock, std::string msg, int reqType);

};


#endif /* LOADBALANCER_H_ */
