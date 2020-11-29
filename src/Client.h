/*
 * Client.h
 *
 *  Created on: Nov 26, 2020
 *      Author: edan
 */

#ifndef CLIENT_H_
#define CLIENT_H_


#include "Connection.h"
#include "Function.h"
#include "Defines"


class Client {
private:

	// menu
	void Menu();

	// run
	void Run();

public:
	Client(std::string indexerAddr, int indexerPort);
	virtual ~Client();

	std::string lb_address;
	int			lb_port;
	int			lb_sock;

};
#endif /* CLIENT_H_ */
