//
// Created by edan on 8.12.2020.
//

#ifndef DKVS_FILEHANDLER_H
#define DKVS_FILEHANDLER_H

#include "Function.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "Defines"

typedef struct request {
    int         GOS;    // 0 for 'set' or 1 for 'get'
    std::string key;
    std::string value;  // optional
}request;

class FileHandler {
private:

public:
    FileHandler();
    ~FileHandler();

    std::vector<request*>   getRequestsFromFile(std::string reqFile);
    std::string             convertFileToString(std::string fileName);
    std::string             convertStringToFile(std::string strFile, std::string fileName);
};

#endif //DKVS_FILEHANDLER_H
