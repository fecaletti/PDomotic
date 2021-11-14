#pragma once
#ifndef _SERVICE_OUTPUT_H
#define _SERVICE_OUTPUT_H

typedef struct ServiceOutput
{
    int out0 = 0;
    int input0 = 0;
    unsigned int execRequestCounter = 0;
    bool automaticLightCommand = false;
};

#endif