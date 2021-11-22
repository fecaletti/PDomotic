from lib.jApi import jApiClient

japi = jApiClient("..\\..\\CPP-Service\\data\\input.json", "..\\..\\CPP-Service\\data\\output.json")

while True:
    command = input("Command [ T / R / S / Q / I / A ]: \n")

    if(command == 'T'):
        print("Stopping...")
        japi.StopThread()
    elif(command == 'R'):
        print("Starting japi...")
        japi.StartThread()
    elif(command == 'S'):
        print(f"JAPI Status = {japi.GetServiceOutput()}")
    elif(command == 'Q'):
        break
    elif(command == 'I'):
        outId = input("Which output id? ")
        targetVal = input("Which value? ")

        ret = japi.SendExecutionRequest(outId = outId, requestedBy = 1, targetValue = targetVal)
        print(f"Returned: {ret}")
    elif(command == 'A'):
        config = { }
        config["key"] = input("Which key?")
        config["value"] = input("Which value?")

        ret = japi.SetServiceConfig(config)
        print(f"Returned = {ret}")
    else:
        print("Not recognized command...")