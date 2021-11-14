#include <iostream>
#include "lib/jApi/jApi.h"

using namespace std;

int main()
{
    StartJapiThread();
    std::string inCmd;
    while(1)
    {
        cout << endl << "Command: [T / S / R / Q]" << endl;

        cin >> inCmd;

        if(inCmd[0] == 'T')
        {
            cout << "Finishing..." << endl;
            StopJapiThread();
        }
        else if(inCmd[0] == 'R')
        {
            cout << "Oppenning again..." << endl;
            StartJapiThread();
        }
        else if(inCmd[0] == 'S')
        {
            cout << "Status = " << GetJapiStatus() << endl;
        }
        else if(inCmd[0] == 'Q') break;
        else cout << "Not recognized command...";
    }

    StopJapiThread();
    return 0;
}