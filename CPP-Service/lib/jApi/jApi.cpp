#include "jApi.h"
#include "../nlohmann/json.hpp"
#include <stdexcept>

using namespace std;
using json = nlohmann::json;

#ifdef WINDOWS_OS
std::thread* apiThread;
#else
pthread_t apiThread;
pthread_attr_t attrApiThread;
void* apiThreadStatus;
#endif

ServiceInput* inputPtr;
ServiceOutput* outPtr;
//std::vector<ExecutionRequest> executeGlobalList(100);
//std::vector<unsigned int> executedRequestsList(100);

bool _runJapiThread = false;
bool _cleaningOrders = false;

json ConvertOutputToJson(ServiceOutput outObj);
bool ParseInput(std::stringstream* strStream, ServiceInput* inObject);
bool ParseExecutionRequestsList(json obj, std::vector<ExecutionRequest>* execList);
// bool InputRequestsContains(unsigned int requestId);
// void CleanExecutedRequests();
#ifdef WINDOWS_OS
int MainAPILoop()
#else
void* MainAPILoop(void* args)
#endif
{
    //inputPtr->executionRequests = &executeGlobalList;
    //outPtr->executedRequests = &executedRequestsList;
    //outPtr->executedRequests->clear();
    //inputPtr->executionRequests->clear();
    JapiPrintln("Started!");
    while (_runJapiThread)
    {
        try
        {
            ReadInput(inputPtr);
            //PrintInput(*inputPtr);
            //outPtr->execRequestCounter = inputPtr->executionRequests->size();
            //outPtr.automaticLightCommand = inputPtr.automaticLightCommand;

            //PrintInput(inputPtr);
            //CleanExecutedRequests();
            WriteOutput(*outPtr);
            #ifdef WINDOWS_OS
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
            #else
            usleep(150000);
            #endif
        }
        catch(const std::exception& e)
        {
            JapiPrintln("EXCEPTION THROWN IN MAIN LOOP");
            std::cerr << e.what() << '\n';
        }
        // catch(const std::out_of_range& o)
        // {
        //     JapiPrintln("OUT OF RANGE EXCEPTION THROWN IN MAIN LOOP");
        //     std::cerr << o.what() << '\n';
        // }
    }
    
    JapiPrintln("Process finished...");

    #ifndef WINDOWS_OS
    pthread_exit(NULL);
    #endif
    return 0;
}

void StartJapiThread(ServiceOutput* ptrOut, ServiceInput* ptrIn)
{
    outPtr = ptrOut;
    inputPtr = ptrIn;
    _runJapiThread = true;
    #ifdef WINDOWS_OS
    apiThread = new std::thread(MainAPILoop);
    #else
    pthread_attr_init(&attrApiThread);
    pthread_attr_setdetachstate(&attrApiThread, PTHREAD_CREATE_JOINABLE);
    int cthread_ret = pthread_create(&apiThread, &attrApiThread, MainAPILoop, NULL);
    #endif
}

void StopJapiThread()
{
    _runJapiThread = false;
    #ifdef WINDOWS_OS
    if(apiThread->joinable())
        apiThread->join();
    #else
    pthread_attr_destroy(&attrApiThread);
    int join_thread_return = pthread_join(apiThread, &apiThreadStatus);
    #endif
}

bool GetJapiStatus()
{
    return _runJapiThread;
}

bool ReadInput(ServiceInput* inObject)
{
    std::ifstream inFile(INPUT_FP);
    std::stringstream strBuffer;

    strBuffer << inFile.rdbuf();

    return ParseInput(&strBuffer, inObject);
}

json ConvertOutputToJson(ServiceOutput outObj)
{
    json jObj;

    jObj["input0"] = outObj.input0;
    jObj["input1"] = outObj.input1;
    jObj["input2"] = outObj.input2;
    jObj["out0"] = outObj.out0;
    jObj["execRequestCounter"] = outObj.execRequestCounter;
    jObj["automaticLightCommand"] = outObj.automaticLightCommand;
    jObj["peopleCounter"] = outObj.peopleCounter;

    //jObj["executedRequests"] = *(outObj.executedRequests);

    //cout << "EXPORTED OUT >> " << jObj["out0"] << " - " << outObj.out0 << endl;
    return jObj;
}

void InsertOutput(int outId, int outVal)
{
    outPtr->out0 = outVal;
}

void InsertInput(int inId, int inVal)
{
    switch(inId)
    {
        case 0:
            outPtr->input0 = inVal;
            break;
        case 1:
            outPtr->input1 = inVal;
            break;
        case 2:
            outPtr->input2 = inVal;
            break;
    }
}

bool ParseInput(std::stringstream* strStream, ServiceInput* inObject)
{
    json jObj = json::parse(strStream->str());
    //inObject->executionRequests->clear();

    inObject->automaticLightCommand = jObj["automaticLightCommand"];
    inObject->debugOnline = jObj["debugOnline"];
    inObject->targetOut0 = jObj["targetOut0"];
    //ParseExecutionRequestsList(jObj["executionRequests"], (inObject->executionRequests));

    return true;
}

bool ParseExecutionRequestsList(json obj, std::vector<ExecutionRequest>* execList)
{
    std::vector<ExecutionRequest>::iterator it;
    bool finished = false;

    it = execList->begin();

    for(json el : obj.items())
    {
        auto specExec = el.begin().value();
        ExecutionRequest request;

        request.idExecution = specExec["idExecution"];
        request.requestedBy = specExec["requestedBy"];
        request.outId = specExec["outId"];
        request.status = specExec["status"];
        request.targetValue = specExec["targetValue"];


        execList->insert(it, request);

        finished = true;
    }
    return finished;
}

void PrintInput(ServiceInput inputObject)
{
    cout << "Input object -->" << endl;
    cout << "automaticLightCommand: " << inputObject.automaticLightCommand << endl;
    cout << "Target out 0: " << inputObject.targetOut0 << endl;

    // for (unsigned i = 0; i < (inputObject.executionRequests)->size(); i++)
    // {
    //     cout << "Execution " << (inputObject.executionRequests)->at(0).idExecution << endl;
    //     cout << "OutId " << (inputObject.executionRequests)->at(0).outId << endl;
    //     cout << "TargetValue " << (inputObject.executionRequests)->at(0).targetValue << endl;
    // }
}

bool WriteInput(ServiceInput inObject)
{
    return false;
}

bool WriteOutput(ServiceOutput outObject)
{
    std::ofstream outFile;
    std::stringstream strStream;

    string serializedData = ConvertOutputToJson(outObject).dump();

    outFile.open(OUTPUT_FP);
    outFile << serializedData;
    outFile.close();

    return true;
}

void JapiPrintln(string data)
{
    cout << "#JAPI --> " << data << endl;
}

int CountExecutedRequests()
{
    return 0;//executedRequestsList.size();
}

void SetPeopleCounter(unsigned int counter)
{
    outPtr->peopleCounter = counter;
}
// void InsertExecutedRequest(unsigned int requestId)
// {
//     std::vector<unsigned int>::iterator it = executedRequestsList.begin();
//     executedRequestsList.insert(it, requestId);
// }

// bool ExecutedRequestsContains(unsigned int requestId)
// {
//     for(unsigned int i = 0; i < executedRequestsList.size(); i++)
//     {
//         if(executedRequestsList.at(i) == requestId)
//             return true;
//     }

//     return false;
// }

// bool InputRequestsContains(unsigned int requestId)
// {
//     for(unsigned i = 0; i < inputPtr->executionRequests->size(); i++)
//     {
//         if((inputPtr->executionRequests)->at(i).idExecution == requestId)
//             return true;
//     }

//     return false;
// }

// void CleanExecutedRequests()
// {
//     _cleaningOrders = true;
//     for(unsigned i = 0; i < executedRequestsList.size(); i++)
//     {
//         std::vector<unsigned int>::iterator it = executedRequestsList.begin();
//         if(!InputRequestsContains(executedRequestsList.at(i)))
//             executedRequestsList.erase(it + i);
//     }
//     _cleaningOrders = false;
// }

bool IsCleaningOrders()
{
    return _cleaningOrders;
}