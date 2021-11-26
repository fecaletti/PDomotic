#pragma once
#ifndef _SERVICE_INPUT_H
#define _SERVICE_INPUT_H

#include <iostream>
#include <vector>
#include "executionRequest.h"

using namespace std;

typedef struct ServiceInput
{
    //std::vector<ExecutionRequest>* executionRequests;
    int automaticLightCommand = 0;
    int debugOnline = 0;
    int targetOut0 = 0;
};


#endif