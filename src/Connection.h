/*
 * Connection.h
 *
 *  Created on: Nov 26, 2020
 *      Author: edan
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include "FileHandler.h"
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


class Connection {

public:

	Connection();
	virtual ~Connection();

	int listen_on(int port);
	int connect_to(std::string address, int port);
    int connect_to(std::string fullAddress);
	int accept_connection();

	int send_uni(int dstSock, std::string msg);
	int send_multi(int* dstSocks, int numDests, std::string msg);
	int send_file(int dstSock, std::string filename);
	int receive(int srcSock, char* buff, int size);
	void disconnect (int sock);


private:

	int 			m_port;
	int				listen_sock;
	std::string 	m_address;

	int StartListeningOn(int port);
	int Connect(std::string address, int port);
	void Disconnect (int sock);
	int Send(int dstSock, std::string msg);
	int Receive(int srcSock, char* buff, int size);
	int Accept(int listener);
};


#endif /* CONNECTION_H_ */
