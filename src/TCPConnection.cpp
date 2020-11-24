/*
 * TCPConnection.cpp
 *
 *  Created on: Oct 18, 2020
 *      Author: edan
 */

#include "TCPConnection.h"

/********************************** CONSTRUCTORS/DESTRUCTOR ***********************************/

TCPConnection::TCPConnection()
	: m_address(std::string("127.0.0.1")), m_port(0), listen_sock(0)
{

}


TCPConnection::~TCPConnection()
{

}

/***************************************** PRIVATE *****************************************/

/* StartListeningOn
 * input arguments: 		port number to be listen on
 * return value:			in success: socket number of listening socket, in failure: -1.
 *
 * function flow:
 * 1. create a socket.
 * 2. Attach the socket into address and port.
 * 3. bind socket to port.
 * 4. listen up to SOMAXCONN connection
 */
int TCPConnection::StartListeningOn(int port)
{

	// open socket
	int welcomSocket = socket (AF_INET, SOCK_STREAM, 0);
	if (welcomSocket == -1)
	{
		perror ("ERROR: cannot create socket\n");
		return -1;
	}

	// attach socket to address and port
	int one = 1;
	if (setsockopt(welcomSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,(const void*)&one, sizeof(int)) < 0)
	{
		perror ("ERROR: cannot reuse address\n");
		close (welcomSocket);
		return -1;
	}

	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl (INADDR_ANY);
	serverAddr.sin_port = htons(port);

	// bind welcome socket
	if ((bind(welcomSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)))!=0)
	{
		perror ("ERROR: cannot bind socket\n");
		close (welcomSocket);
		return -1;
	}

	// listen to port
	if ((listen(welcomSocket, SOMAXCONN)) != 0)
	{
		perror ("ERROR: cannot listen\n");
		close (welcomSocket);
		return -1;
	}
	return welcomSocket;
}

/* Connect
 * input arguments: 		adrress and port number to be connected to
 * return value:			in success: socket number of server, in failure: -1.
 *
 * function flow:
 * 1. create a socket.
 * 2. connect to server.
 */
int TCPConnection::Connect(std::string address, int port)
{

	// create a socket
	int sock = socket (AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		perror ("ERROR: cannot create socket\n");
		return -1;
	}

	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = inet_addr(address.c_str());

	// connect to server
	if ((connect(sock,(struct sockaddr*)&serverAddr, sizeof(serverAddr))) != 0)
	{
		printf ("ERROR: cannot connect to %s:%d\n", address.c_str(), port);
		close (sock);
		return -1;
	}
	return sock;
}

int TCPConnection::Accept(int listener)
{
	struct sockaddr_in clientAddr;
	int addrLen = sizeof(clientAddr);
	int clientSock;
	if ((clientSock = accept(listener,(struct sockaddr*)&clientAddr, (socklen_t*)&addrLen)) < 0){
		std::cerr << "ERROR: cannot accept" << std::endl;
		return 0;
	}
	return clientSock;
}

void TCPConnection::Disconnect(int sock)
{
	close (sock);
}

int TCPConnection::Send(int dstSock, std::string msg)
{

	int byteSent = 0, totalByteSent = 0;

	while (totalByteSent < msg.size())
	{
		byteSent = send (dstSock, msg.c_str(), msg.size() - totalByteSent, 0);
		if (byteSent < 0)
		{
			std::cerr << "ERROR: cannot send" << std::endl;
			break;
		}
		totalByteSent += byteSent;
	}

	send (dstSock, DONE, sizeof(DONE), 0);

	return totalByteSent;
}

int TCPConnection::Receive(int srcSock, char* buff, int size)
{

	memset (buff, '\0', size);
	int byteReceived = 0, totalByteReceived = 0;
	bool runFlag = true;

	while (size > totalByteReceived && runFlag)
	{
		byteReceived = recv (srcSock,&buff[totalByteReceived], size - totalByteReceived, 0);

		switch (byteReceived)
		{
		case -1:
			std::cerr << "ERROR: cannot receive" << std::endl;
			return -1;

		case 0:
			std::cout << "Remote peer disconnected" << std::endl;
			return 0;

		default:
			std::string request = std::string(buff);
			std::string doneStr = request.substr(request.size() - sizeof(DONE) + 1, sizeof(DONE));
			if (doneStr.compare(DONE) == 0)
			{
				request = request.substr(0, request.size() - sizeof(DONE) + 1);
				memset (buff, '\0', size);
				memcpy (buff, request.c_str(), request.size());
				runFlag = false;
			}
			totalByteReceived += byteReceived;
			break;
		}

	}

	return totalByteReceived;
}

/***************************************** PUBLIC *****************************************/

int TCPConnection::send_uni(int dstSock, std::string msg)
{

	int sizeSent = Send (dstSock, msg);
	if (sizeSent != msg.size())
	{
		std::cout << "WARNING: not all data sent successfully" << std::endl
				  << msg.c_str() << " of size " << msg.size() << " total sent: " << sizeSent << std::endl;
		return 1;
	}

	return 0;
}

int TCPConnection::send_multi(int* dstSocks, int numDests, std::string msg)
{

	if (msg.size() > MAX_DATA_SIZE)
	{
		std::cout << "ERROE: cannot send such a large message" << std::endl;
		return 1;
	}

	int sizeSent;
	for (int i=0; i < numDests; i++)
	{
		sizeSent = Send (dstSocks[i], msg);
		if (sizeSent != msg.size())
		{
			std::cout << "WARNING: not all data sent successfully" << std::endl;
			return 1;
		}
	}
	return 0;

}

int TCPConnection::receive(int srcSock, char* buff, int size)
{
	if (size > MAX_DATA_SIZE)
	{
		std::cout << "ERROR: cannot receive such a large message" << std::endl;
		return 1;
	}

	int sizeReceived = Receive(srcSock, buff, size);
	if (sizeReceived <= 0)
	{
		return 1;
	}
	return 0;
}

int TCPConnection::listen_on(int port)
{
	m_port = port;
	listen_sock = StartListeningOn(port);
	return listen_sock;
}

int TCPConnection::connect_to(std::string address, int port)
{
	m_address = address;
	m_port = port;
	return Connect(address, port);
}

int TCPConnection::accept_connection()
{
	return Accept(listen_sock);
}

void TCPConnection::disconnect (int sock)
{
	Disconnect(sock);
}

