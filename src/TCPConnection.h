/*
 * TCPConnection.h
 *
 *  Created on: Oct 18, 2020
 *      Author: edan
 */

#include <iostream>			/* cout, cin, cerr */
#include <stdio.h>
#include <stdlib.h>			/* atoi */
#include <vector>			/* vector */

#include <sys/socket.h>
#include <sys/types.h>		/* send, recv */

#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include <errno.h>
#include <time.h>
#include <math.h>
#include <pthread.h>
#include <string.h>
#include "Defines"

#ifndef TCPCONNECTION_H_
#define TCPCONNECTION_H_

//class TCPConnection;

//// callback for received messages
//typedef void(*MessageHanlder)(TCPConnection* conn, int sock, std::string msg);

class TCPConnection
{

public:

	TCPConnection();
//	TCPConnection(MessageHanlder handler);
	virtual ~TCPConnection();

	int listen_on(int port);
	int connect_to(std::string address, int port);
	int accept_connection();

	int send_uni(int dstSock, std::string msg);
	int send_multi(int* dstSocks, int numDests, std::string msg);
	int receive(int srcSock, char* buff, int size);
	void disconnect (int sock);


private:

	int 			m_port;
	int				listen_sock;
	std::string 	m_address;
//	MessageHanlder	MSGHandler;

	int StartListeningOn(int port);
	int Connect(std::string address, int port);
	void Disconnect (int sock);
	int Send(int dstSock, std::string msg);
	int Receive(int srcSock, char* buff, int size);
	int Accept(int listener);
};

#endif /* TCPCONNECTION_H_ */
