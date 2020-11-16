/*
 * DKVSClient.h
 *
 *  Created on: Nov 8, 2020
 *      Author: edan
 */

#include "TCPConnection.h"
#include "GeneralFunctions.h"
#include "Defines"

#ifndef DKVSCLIENT_H_
#define DKVSCLIENT_H_

class DKVS_Client {
private:

	// menu
	void Menu();

	// run
	void Run();

public:
	DKVS_Client(std::string indexerAddr, int indexerPort);
	virtual ~DKVS_Client();

	std::string lb_address;
	int			lb_port;
	int			lb_sock;

};

#endif /* DKVSCLIENT_H_ */
