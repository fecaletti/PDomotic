#include "jApi.h"
#include "../nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;

std::thread* apiThread;

ServiceInput inputData;
ServiceOutput outputData;
std::vector<ExecutionRequest> executeGlobalList(100);

bool _runJapiThread = false;

json ConvertOutputToJson(ServiceOutput outObj);
bool ParseInput(std::stringstream* strStream, ServiceInput* inObject);
bool ParseExecutionRequestsList(json obj, std::vector<ExecutionRequest>* execList);

int MainAPILoop()
{
    inputData.executionRequests = &executeGlobalList;
    JapiPrintln("Started!");
    while (_runJapiThread)
    {
        ReadInput(&inputData);

        outputData.execRequestCounter = inputData.executionRequests->size();
        //outputData.automaticLightCommand = inputData.automaticLightCommand;

        //PrintInput(inputData);
        WriteOutput(outputData);
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
    
    JapiPrintln("Process finished...");
    return 0;
}

void StartJapiThread()
{
    _runJapiThread = true;
    apiThread = new std::thread(MainAPILoop);
}

void StopJapiThread()
{
    _runJapiThread = false;
    if(apiThread->joinable())
        apiThread->join();
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
    jObj["out0"] = outObj.out0;
    jObj["execRequestCounter"] = outObj.execRequestCounter;
    jObj["automaticLightCommand"] = outObj.automaticLightCommand;

    return jObj;
}

bool ParseInput(std::stringstream* strStream, ServiceInput* inObject)
{
    json jObj = json::parse(strStream->str());
    inObject->executionRequests->clear();

    inObject->automaticLightCommand = jObj["automaticLightCommand"];
    ParseExecutionRequestsList(jObj["executionRequests"], (inObject->executionRequests));

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
        // cout << "INNER OBJECTS" << endl;
        // for (auto& sel : el.begin().value().items())
        // {
        //     cout << sel.key() << " : " << sel.value() << endl;
        // }
    }
    return finished;
}

void PrintInput(ServiceInput inputObject)
{
    cout << "Input object -->" << endl;
    cout << "automaticLightCommand: " << inputObject.automaticLightCommand << endl;
    for (unsigned int i = 0; i < (inputObject.executionRequests)->size(); i++)
    {
        cout << "Execution " << (inputObject.executionRequests)->at(0).idExecution << endl;
        cout << "OutId " << (inputObject.executionRequests)->at(0).outId << endl;
        cout << "TargetValue " << (inputObject.executionRequests)->at(0).targetValue << endl;
    }
    
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
    cout << endl << "#JAPI --> " << data;
}