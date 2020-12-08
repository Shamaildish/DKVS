//
// Created by edan on 8.12.2020.
//

#include "FileHandler.h"

FileHandler::FileHandler(char *fileName) {
    file.open(fileName);

    // cannot open file
    if (!file)
    {
        std::cout << "ERROR: cannot open file " << fileName << std::endl;
    }
}

FileHandler::~FileHandler() {

}

std::vector<request*> FileHandler::getRequestsFromFile() {

    std::vector<request*> requests(0);
    bool firstState = true;
    bool secondState = false;
    bool thirdState = false;
    std::string line;
    request *req;

    while (getline(file, line))
    {
        if (firstState)
        {
            // check validation of request type
            if (line.compare("s") && line.compare("g"))
            {
                std::cout << "ERROR: wrong request type in file" << std::endl;
                return std::vector<request*>();
            }

            // create a new struct
            req = new request();

            // set request type to struct
            (!line.compare("s")) ? req->GOS = SET : req->GOS = GET;

            // change state
            firstState = !firstState;
            secondState = !secondState;
        }
        else if (secondState)
        {

            // change state
            if (!line.compare("%")) {
                secondState = !secondState;
                thirdState = !thirdState;
                continue;
            }

            // change state
            if (!line.compare("%%")) {
                requests.push_back(req);

                secondState = !secondState;
                firstState = !firstState;
                continue;
            }

            // add line to key
            req->key.append(line).append("\n");
        }
        else if (thirdState)
        {
            // add request to list and change state
            if (!line.compare("%%")) {
                requests.push_back(req);

                thirdState = !thirdState;
                firstState = !firstState;;
                continue;
            }

            // add line to value
            req->value.append(line).append("\n");
        }
    }
    requests.push_back(req);
    return requests;
}
