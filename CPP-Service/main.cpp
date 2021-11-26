#include <iostream>
#include "lib/jApi/jApi.h"

#if defined(WIN32) || (_WIN32)
#define WINDOWS_OS
#include <thread>
#include <chrono>
#else
#include <unistd.h>
#endif

using namespace std;

/* DEBUG DEFINES */
#define INPUT 0
#define OUTPUT 1
#define LED_BUILTIN 1
#define RISING 1
#define HIGH 1
#define LOW 0
#define pinMode(a , b) { cout << "PINMODE-> " << a << ", " << b << endl; }
#define attachInterrupt(a , b, c) { cout << "ATTACHINTERRUPT-> " << a << ", " << b << ", " << c << endl; } 


#define SENSOR_1_PIN 2
#define SENSOR_1_INT_ID 0
#define SENSOR_2_PIN 3
#define SENSOR_2_INT_ID 1
#define OUT_PIN 6
#define MAIN_LOOP_INTERVAL 50 //ms

int peopleInTheRoom = 0;
int sens1On = 0;
int sens2On = 0;
int out0On = 0;
int input0On = 0;

volatile unsigned long timeStampSens1 = 0;
volatile unsigned long timeStampSens2 = 0;

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

int digitalRead(int a);
void digitalWrite(int a, int b);

int main()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(SENSOR_1_PIN, INPUT);
    pinMode(SENSOR_2_PIN, INPUT);
    pinMode(OUT_PIN, OUTPUT);

    attachInterrupt(SENSOR_1_INT_ID, Sens1Callback, RISING);
    attachInterrupt(SENSOR_2_INT_ID, Sens2Callback, RISING);

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

int digitalRead(int a)
{
  if(a == 2) return sens1On;
  else return sens2On;
}

void digitalWrite(int a, int b)
{
  out0On = b;
}

void Sens1Callback()
{
  sens1On = true;
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