import os
from threading import Thread
import json
import numpy as np
import datetime

SERVICE_INPUT_FP = "../../CPP-Service/data/input.json"
SERVICE_OUTPUT_FP = "../../CPP-Service/data/output.json"

class ServiceInput:
    def __init__(self):
        self.executionRequests = []
        self.automaticLightCommand = True
        self.debugOnline = False


def CreateExecutionRequest(idExecution, requestedBy, status, outId, targetValue):
    req = {
        'idExecution': idExecution,
        'requestedBy': requestedBy,
        'status': status,
        'outId': outId,
        'targetValue': targetValue
    }

    return req

class jApiClient:
    def __init__(self, serviceInputFP = SERVICE_INPUT_FP, serviceOutputFP = SERVICE_OUTPUT_FP):
        self.__serviceInputFP = serviceInputFP
        self.__serviceOutputFP = serviceOutputFP
        self.__loopRunning = True
        self.__executionRequests = []
        self.__serviceInputObj = ServiceInput()
        self.__serviceOutputObj = None
        self.__serviceInputNeedSync = True
        self.__internalThread = None
        self.__errorLog = []
        self.__checkIfCanRun()
        self.__startThread()
    
    def __checkIfCanRun(self):
        if(os.path.exists(self.__serviceInputFP) and os.path.exists(self.__serviceOutputFP)):
            self.canRun = True
        else:
            self.canRun = False
    
    def __startThread(self):
        if(self.__internalThread == None):
            self.__internalThread = Thread(target = self.__mainJApiLoop, args = [])
        elif(self.__internalThread.is_alive()):
            print("#JAPI> Waiting task end...")
            self.__internalThread.join()

        if(self.canRun):
            self.__internalThread.start()
        else:
            print("#JAPI> Thread couldn't start...")

    def __mainJApiLoop(self):
        print("#JAPI> Thread started!")
        while self.__loopRunning:
            if self.canRun:
                self.__syncServiceOutput()
                if self.__serviceInputNeedSync:
                    self.__syncServiceInput()
                    self.__serviceInputNeedSync = False
            else:
                self.__checkIfCanRun()
        print("#JAPI> Ending jApi thread...")
        
    def __syncServiceInput(self):
        try:
            self.__serviceInputObj.executionRequests = self.__executionRequests
            print(self.__serviceInputObj.__dict__)
            with open(self.__serviceInputFP, "w") as siFile:
                siFile.write(json.dumps(self.__serviceInputObj.__dict__, indent = 4, sort_keys = True))
        except:
            self.__addErrorLog('Sync SI file', 'Error in write method')

    def __syncServiceOutput(self):
        try:
            with open(self.__serviceOutputFP, "r") as soFile:
                soCpltString = soFile.read()
                self.__serviceOutputObj = json.loads(soCpltString)
        except:
            self.__addErrorLog('Sync Service Output', 'Service output reading method error.')

    def __addErrorLog(self, session, error):
        self.__errorLog.append(f'#JAPI_ERROR> {session}: {error} --- {datetime.datetime.now}')
        self.__exportErrorLog()

    def __exportErrorLog(self):
        with open("japy_err.japy", "w+") as errFile:
            errFile.writelines(self.__errorLog)
        self.__errorLog.clear()

    def GetNextExecutionId(self):
        if(len(self.__executionRequests) == 0):
            return 1
        
        executionIds = [i["idExecution"] for i in self.__executionRequests]
        return np.max(executionIds) + 1

    def SendExecutionRequest(self, requestedBy, outId, targetValue):
        idExecution = self.GetNextExecutionId()
        self.__executionRequests.append(CreateExecutionRequest(int(idExecution), requestedBy, 0, int(outId), int(targetValue)))
        self.__serviceInputNeedSync = True
        return True

    def SetServiceConfig(self, config):
        if(hasattr(self.__serviceInputObj, config.key)):
            setattr(self.__serviceInputObj, config.key, config.value)
            self.__serviceInputNeedSync = True
            return True
        else:
            return False

    def GetServiceOutput(self):
        return self.__serviceOutputObj

    def GetServiceInput(self):
        return self.__serviceInputObj

    def StopThread(self):
        self.__loopRunning = False

    def StartThread(self):
        self.__loopRunning = True
        self.__startThread()