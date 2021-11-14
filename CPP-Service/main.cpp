#include <iostream>
#include <chrono>
#include <thread>
#include "lib/jApi/jApi.h"

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
#define MAIN_LOOP_INTERVAL 300 //ms

int peopleInTheRoom = 0;
volatile bool sens1On = false;
volatile bool sens2On = false;
volatile bool out0On = false;
volatile bool input0On = false;

volatile unsigned long timeStampSens1 = 0;
volatile unsigned long timeStampSens2 = 0;

void Sens1Callback();
void Sens2Callback();
void TreatOutputData(ServiceOutput* output, ServiceInput input);
void TreatInputData(ServiceInput input, ServiceOutput* output);
void ReadDigitalInputs();
void ExecuteOrder(ExecutionRequest* order);
void DefaultLog(string message);

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

    StartJapiThread();

    DefaultLog("Started! Entering main loop...");
    while(1)
    {
      //digitalWrite(LED_BUILTIN, HIGH);
      //_sleep(1000); // Wait for 1000 millisecond(s)
      //digitalWrite(LED_BUILTIN, LOW);
      //_sleep(1000); // Wait for 1000 millisecond(s)

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

      std::this_thread::sleep_for(std::chrono::milliseconds(MAIN_LOOP_INTERVAL));
    }

    return 0;
}

void TreatInputData(ServiceInput input, ServiceOutput* output)
{
  output->automaticLightCommand = input.automaticLightCommand;
  for (unsigned int i = 0; i < output->execRequestCounter; i++)
  {
    ExecuteOrder(&input.executionRequests->at(i));
  }
}

void ExecuteOrder(ExecutionRequest* order)
{
  if((order->status == ExecutionStatus :: WAITING) && (!ExecutedRequestsContains(order->idExecution)))
  {
    digitalWrite(order->outId, order->targetValue);
    InsertExecutedRequest(order->idExecution);
    DefaultLog("Executed order: " + to_string(order->idExecution));
  }
}

void ReadDigitalInputs()
{
  input0On = true; //DigitalRead(input0);
}

void TreatOutputData(ServiceOutput* output, ServiceInput input)
{
  output->automaticLightCommand = input.automaticLightCommand;

  output->input0 = input0On;
  output->input1 = sens1On;
  output->input2 = sens2On;

  output->out0 = out0On;
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