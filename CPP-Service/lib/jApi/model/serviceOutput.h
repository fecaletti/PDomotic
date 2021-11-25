#pragma once
#ifndef _SERVICE_OUTPUT_H
#define _SERVICE_OUTPUT_H

#include <iostream>
#include <vector>

using namespace std;

typedef struct ServiceOutput
{
    int out0 = 0;
    int input0 = 0;
    int input1 = 0;
    int input2 = 0;
    unsigned int execRequestCounter = 0;
    int automaticLightCommand = false;
    std::vector<unsigned int>* executedRequests;
};

#endif