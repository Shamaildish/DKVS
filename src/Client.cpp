/*
 * DKVSClient.cpp
 *
 *  Created on: Nov 8, 2020
 *      Author: edan
 */

#include "Client.h"
#include "FileHandler.h"

int sendRequestsFilesToServers(std::string fileName);

Client::Client(std::string lbAddr, int lbPort)
: lb_address(lbAddr), lb_port (lbPort), lb_sock(0)
{
	Run();
}

Client::~Client()
{

}

/*					PRIVATE					*/

// menu
void 			Client::Menu()
{

	std::cout	<< std::endl
			<< "		MENU" << std::endl
			<< "		----" << std::endl
			<< std::endl
			<< "	-------------------------" << std::endl
			<< "	| enter 's' to set	|" << std::endl
			<< "	| enter 'g' to get	|" << std::endl
			<< "	| enter 'q' to quite    |" << std::endl
			<< "	-------------------------" << std::endl
			<< std::endl;
}

// run
void 			Client::Run()
{

	Function g;
	char buff[MAX_DATA_SIZE];
//	char file[MAX_FILE_SIZE];
	std::string key;
	std::string value;
	std::string fileName;
	std::string respond("");
	char usrInput;
	Connection lbConn;

	// while user not quieting
	while (1)
	{
		// show menu
		Menu();

		// get user input
		std::cin >> usrInput;

		// quieting
		if (usrInput == 'q')
		{
			std::cout << std::endl << "	Bye Bye =]" << std::endl;
			return;
		}

        else if (usrInput == 'f')
        {
            // get file name
            std::cout << "Enter file name: ";
            std::cin >> fileName;
            lbConn.connect_to(lb_address, lb_port);
            lbConn.send_file(lb_sock, fileName);

            // receive file from lb
            lbConn.receive(lb_sock, buff, MAX_FILE_SIZE);

            sendRequestsFilesToServers(buff);
            continue;
        }

		// get key
		std::cout << "Enter key: ";
		std::cin >> key;

		// generate message to indexer
		/* request = <request-type>~<key> */
		sprintf (buff, "%d~%s\0", SET, key.c_str());

		// send message to indexer
        lb_sock = lbConn.connect_to(lb_address, lb_port);
		lbConn.send_uni(lb_sock, std::string(buff));

		// wait for indexer respond
		memset(buff, '\0', MAX_DATA_SIZE);
		lbConn.receive(lb_sock, buff, MAX_DATA_SIZE);
        lbConn.disconnect(lb_sock);

		if (atoi((char*)&buff[0]) != SET)
		{
			std::cout << "ERROR: wrong message type from lb" << std::endl;
			exit(0);
		}

		// get server info
		/* buff = <request-type>~<server-address>:<server-port> */
		std::vector<std::string> splitted = g.split(std::string(buff).substr(2,std::string(buff).size()), ':');
		std::string serverAddr = splitted[0];
		int serverPort = atoi(splitted[1].c_str());

		// connect to server
		Connection serverConn;
		int serverSock = serverConn.connect_to(serverAddr, serverPort);

		// set
		if (usrInput == 's')
		{

			// get value
			std::cout << "Enter value: ";
			std::cin >> value;

			// generate request to server
			memset(buff, '\0', MAX_DATA_SIZE);
			sprintf (buff, "%d~%s:%s\0", SET, key.c_str(), value.c_str());

			// send request to server
			serverConn.send_uni(serverSock, std::string(buff));

			// wait for server respond
			memset (buff, '\0', MAX_DATA_SIZE);
			serverConn.receive(serverSock, buff, MAX_DATA_SIZE);

			// parse respond
			/* buff = <request-type>~<key>:<ack> */
			std::vector<std::string> serverRes = g.split(std::string(buff), '~');
			std::vector<std::string> respond = g.split(serverRes[1], ':');
			if (atoi(serverRes[0].c_str()) != SET || std::string(key).compare(respond[0]) || respond[1].compare("ACK"))
			{
				std::cout << "ERROR: server did not added the key to db" << std::endl;
				continue;
			}
			std::cout << key.c_str() << " was added to " << serverAddr.c_str() << ":" << serverPort << std::endl;
		}

		else if (usrInput == 'g')
		{
			// generate request to server
			memset (buff, '\0', MAX_DATA_SIZE);
			sprintf (buff, "%d~%s\0", GET, key.c_str());

			// send request to server
			serverConn.send_uni(serverSock, std::string(buff));

			// wait for server respond
			memset (buff, '\0', MAX_DATA_SIZE);
			serverConn.receive(serverSock, buff, MAX_DATA_SIZE);

			// parse respond
			/* buff = <request-type>~<key>:<value> */
			std::vector<std::string> serverRes = g.split(std::string(buff), '~');
			std::vector<std::string> respond = g.split(serverRes[1], ':');
			if (atoi(serverRes[0].c_str()) != GET || std::string(key).compare(respond[0]))
			{
				std::cout << "ERROR: server does not hold the key" << std::endl;
				return;
			}
			std::cout << "the value of key '" << key << "' is '" << respond[1] << "'" << std::endl;
		}

		else
		{
			// wrong input
			std::cout << std::endl << "wrong input, try again" << std::endl << std::endl;
		}

		lbConn.disconnect(lb_sock);

	}
}


/*
 * This func is being called after receiving file respond from the load balancer with addresses of servers as values to the keys
 */
int Client::sendRequestsFilesToServers(std::string fileName) {

    FileHandler fHandler;
    std::vector<request*> requests = fHandler.getRequestsFromFile(fileName);

    // the value of the request is the server address who holds the key
    std::string serverInfo = requests[0]->value;

    // create a list of queues for requests
    std::vector<std::vector<request*>> queues;

    // sort request by server info into queues
    for (request* req : requests)
    {
        bool pushed = false;
        for (std::vector<request*> queue : queues)
        {
            if (!serverInfo.compare(queue[0]->value))
            {
                queue.push_back(req);
                pushed = true;
                break;
            }
        }

        if (!pushed)
        {
            std::vector<request*> q;
            q.push_back(req);
            queues.push_back(q);
        }
    }

    Connection serverConn;
    std::vector<int> serverSocks;
    int serverSock;

    // create file for each server
    std::ofstream fileForServer;
    char serverFileName[32];
    for (std::vector<request*> queue : queues)
    {
        sprintf (serverFileName, "req_server_%s\0",queue[0]->value.c_str());
        fileForServer.open(serverFileName);
        for (request* req : queue)
        {
            fileForServer << req->GOS << "\n" << req->key.c_str() << "\n%\n";
            if (req->GOS == SET)
            {
                fileForServer << req->value.c_str() << "\n";
            }
            fileForServer << "%%\n";
        }
        fileForServer.close();

        // send file to server
        serverSock = serverConn.connect_to(queue[0]->value);
        serverSocks.push_back(serverSock);
        serverConn.send_file(serverSock, serverFileName);
    }

    // wait to receive a respond file from all servers
    char file[MAX_FILE_SIZE];
    for (int srv=0; srv<queues.size(); srv++)
    {
        // receive msg from server
        memset(file, 0, MAX_FILE_SIZE);
        serverConn.receive(serverSocks[srv], file, MAX_FILE_SIZE);

        // convert msg to file
        sprintf (serverFileName, "res_server_%s\0",queues[srv][0]->value.c_str());
        fHandler.convertStringToFile(std::string(file), serverFileName);
    }
    return 0;
}

