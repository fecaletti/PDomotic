#include <iostream>
#include "lib/jApi/jApi.h"

#if defined(WIN32) || (_WIN32)
#define WINDOWS_OS
#include <thread>
#include <chrono>
#else
#include <unistd.h>
#include <wiringPi.h>
#endif

using namespace std;

/* DEBUG DEFINES --> MOCK WIRING PI */
#ifdef WINDOWS_OS

#define INPUT 0
#define OUTPUT 1
#define LED_BUILTIN 1
#define RISING 1
#define HIGH 1
#define LOW 0
#define pinMode(a , b) { cout << "PINMODE-> " << a << ", " << b << endl; }
#define attachInterrupt(a , b, c) { cout << "ATTACHINTERRUPT-> " << a << ", " << b << ", " << c << endl; }

#endif


#define SENSOR_1_PIN 2 //PIN 7 HEAD 3.3V -- PIN13 / BCM 21
#define SENSOR_2_PIN 3 //PIN 8 HEAD 3.3V -- PIN15 / BCM 22
#define OUT_PIN 7 //PIN 4 HEAD 3.3V -- PIN 7 / BCM 4
#define MAIN_LOOP_INTERVAL 50 //ms
#define SENS1_DEAD_TIME 20 // * main loop interval = 50ms   

int peopleInTheRoom = 0;
int sens1On = 0;
int sens2On = 0;
int out0On = 0;
int input0On = 0;

volatile unsigned long timeStampSens1 = 0;
volatile unsigned long timeStampSens2 = 0;
volatile unsigned long timeStampGeneral = 0;

ServiceOutput outputData;
ServiceInput inputData;

void Sens1Callback();
void Sens2Callback();
void TreatOutputData(ServiceOutput* output, ServiceInput input);
void TreatInputData(ServiceInput input, ServiceOutput* output);
void ReadDigitalInputs();
void ExecuteOrder(ExecutionRequest* order);
void DefaultLog(string message);
void SetOutputs(ServiceInput input);

#ifdef WINDOWS_OS
int digitalRead(int a);
void digitalWrite(int a, int b);
#endif

int main()
{
  #ifndef WINDOWS_OS
  wiringPiSetup();
  #endif
  pinMode(SENSOR_1_PIN, INPUT);
  pinMode(SENSOR_2_PIN, INPUT);
  pinMode(OUT_PIN, OUTPUT);

  #ifndef WINDOWS_OS
  wiringPiISR(SENSOR_1_PIN, INT_EDGE_FALLING, Sens1Callback);
  wiringPiISR(SENSOR_2_PIN, INT_EDGE_FALLING, Sens2Callback);
  #endif

  StartJapiThread(&outputData, &inputData);

  DefaultLog("Started! Entering main loop...");
  while(1)
  {

    if(inputData.debugOnline)
    {
      DefaultLog("Pessoas no quarto: " + to_string(peopleInTheRoom));
      DefaultLog("Estado da lâmpada: " + to_string(digitalRead(OUT_PIN)));
      DefaultLog(to_string(sens1On) + " - " + to_string(sens2On));
      DefaultLog(to_string(digitalRead(SENSOR_1_PIN)) + " - " + to_string(digitalRead(SENSOR_2_PIN)));
    }

    if(inputData.automaticLightCommand)
    {
      if(peopleInTheRoom > 0) digitalWrite(OUT_PIN, HIGH);
      else digitalWrite(OUT_PIN, LOW);  
    }

    TreatInputData(inputData, &outputData);
    TreatOutputData(&outputData, inputData);

    timeStampGeneral++;
    
    if(sens1On && ((timeStampGeneral - timeStampSens1) >= SENS1_DEAD_TIME))
      sens1On = false;

    if(sens2On && ((timeStampGeneral - timeStampSens2) >= SENS1_DEAD_TIME))
      sens2On = false;
    #ifdef WINDOWS_OS
    std::this_thread::sleep_for(std::chrono::milliseconds(MAIN_LOOP_INTERVAL));
    #else
    usleep(MAIN_LOOP_INTERVAL * 1000);
    #endif
  }

  return 0;
}

void TreatInputData(ServiceInput input, ServiceOutput* output)
{
  output->automaticLightCommand = input.automaticLightCommand;
  SetOutputs(input);
  // if(IsCleaningOrders())
  //   return;
  // for (unsigned int i = 0; i < output->execRequestCounter; i++)
  // {
  //   ExecuteOrder(&input.executionRequests->at(i));
  // }
}

void SetOutputs(ServiceInput input)
{
  digitalWrite(0, input.targetOut0);
  InsertOutput(0, out0On);
}

void ExecuteOrder(ExecutionRequest* order)
{
  // if((order->status == ExecutionStatus :: WAITING) && (!ExecutedRequestsContains(order->idExecution)))
  // {
  //   digitalWrite(order->outId, order->targetValue);
  //   InsertOutput(0, order->targetValue);
  //   //InsertExecutedRequest(order->idExecution);
  //   DefaultLog("OUT VAL ->" + to_string(out0On) + " - " + to_string(order->targetValue));
  //   DefaultLog("Executed order: " + to_string(order->idExecution));
  // }
}

void ReadDigitalInputs()
{
  input0On = true; //DigitalRead(input0);
  InsertInput(0, 1);
  InsertInput(1, 1);
  InsertInput(2, 1);
}

void TreatOutputData(ServiceOutput* output, ServiceInput input)
{
  output->automaticLightCommand = input.automaticLightCommand;

  output->input0 = input0On;
  output->input1 = sens1On;
  output->input2 = sens2On;
  SetPeopleCounter(peopleInTheRoom);
  //output->out0 = out0On;
}

void Sens1Callback()
{
  sens1On = true;
  timeStampSens1 = timeStampGeneral;
  if(sens2On)
  {
    if(peopleInTheRoom > 0) peopleInTheRoom--;
    sens1On = false;
    sens2On = false;
  }
}

void Sens2Callback()
{
  sens2On = true;
  timeStampSens2 = timeStampGeneral;
  if(sens1On)
  {
    peopleInTheRoom++;
    sens1On = false;
    sens2On = false;
  }
}

void DefaultLog(string message)
{
  cout << "#CPP-SERVICE> " << message << endl;
}

/* WiringPi MOCK */
#ifdef WINDOWS_OS

int digitalRead(int a)
{
  if(a == 2) return sens1On;
  else return sens2On;
}

void digitalWrite(int a, int b)
{
  out0On = b;
}

#endif
