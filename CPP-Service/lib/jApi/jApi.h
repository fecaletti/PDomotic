#ifndef _JAPI_H_
#define _JAPI_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <time.h>

#if defined(_WIN32) || defined(WIN32) 
#include <thread>
#include <chrono>
#define WINDOWS_OS
#else
#include <pthread.h>
#include <unistd.h>
#endif

#include "model/executionRequest.h"
#include "model/serviceInput.h"
#include "model/serviceOutput.h"

#define INPUT_FP "data/input.json"
#define OUTPUT_FP "data/output.json"

using namespace std;

void StartJapiThread(ServiceOutput* ptrOut, ServiceInput* ptrIn);
void StopJapiThread();
bool GetJapiStatus();
bool ReadInput(ServiceInput* inObject);
void PrintInput(ServiceInput inputObject);
bool WriteInput(ServiceInput inObject);
bool WriteOutput(ServiceOutput outObject);
void JapiPrintln(string data);
//bool ExecutedRequestsContains(unsigned int requestId);
//void InsertExecutedRequest(unsigned int requestId);
void InsertOutput(int outId, int outVal);
void InsertInput(int inId, int inVal);
void SetPeopleCounter(unsigned int counter);
int CountExecutedRequests();
bool IsCleaningOrders();

// extern ServiceInput inputData;
// extern ServiceOutput outputData;
#endif