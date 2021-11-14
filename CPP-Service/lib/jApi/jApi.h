#ifndef _JAPI_H_
#define _JAPI_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <time.h>
#include <chrono>
#include <thread>

#include "model/executionRequest.h"
#include "model/serviceInput.h"
#include "model/serviceOutput.h"

#define INPUT_FP "data/input.json"
#define OUTPUT_FP "data/output.json"

using namespace std;

void StartJapiThread();
void StopJapiThread();
bool GetJapiStatus();
bool ReadInput(ServiceInput* inObject);
void PrintInput(ServiceInput inputObject);
bool WriteInput(ServiceInput inObject);
bool WriteOutput(ServiceOutput outObject);
void JapiPrintln(string data);

extern ServiceInput inputData;
extern ServiceOutput outputData;

#endif