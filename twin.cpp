#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iphlpapi.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>


#pragma comment(lib, "Ws2_32.lib")

#define MUTEX_NAME "technai_mutex"
#define RUN_REG "Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define PROGRAM_NAME "Technai"
#define PROGRAM_PATH "C:\\Users\\User\\source\\repos\\twin\\x64\\Debug\\twin.exe"

enum WinapiError
{
    winapiError,
    otherError
};

const char* lastWinapiFunction = "";
std::vector<HANDLE> handles;
int lastError = 0;

HANDLE ensureOneOrogram()
{
    HANDLE mutex = CreateMutexA(
        NULL,
        TRUE,
        MUTEX_NAME
    );

    if (mutex == NULL)
    {
        std::cout << "error number " << GetLastError() << " in CreateMutexA\n";
        exit(1);
    }

    DWORD waitStatus = WaitForSingleObject(mutex, 0);
    if (waitStatus == WAIT_OBJECT_0)
    {
        handles.push_back(mutex);
        return mutex;
    }
    else if (waitStatus == WAIT_ABANDONED || waitStatus == WAIT_TIMEOUT)
    {
        std::cout << "program is already running...\n";
        exit(1);
    }
    else if (waitStatus == waitStatus)
    {
        std::cout << "error number " << GetLastError() << " in WaitForSingleObject\n";
        exit(1);
    }
    else
    {
        std::cout << "Unkwon error\n";
        exit(1);
    }

}



void RunOnStartUp()
{
    HKEY hkey;

    LSTATUS s = RegCreateKeyA(
        HKEY_CURRENT_USER,
        RUN_REG,
        &hkey
    );

    if (s != ERROR_SUCCESS)
    {
        lastWinapiFunction = "RegCreateKeyA";
        lastError = s;
        throw otherError;
    }

    s = RegSetKeyValueA(
        hkey, 
        NULL,
        PROGRAM_NAME, 
        REG_SZ,
        PROGRAM_PATH, 
        (DWORD)strlen(PROGRAM_PATH) + 1
    );
    RegCloseKey(hkey);
    if (s != ERROR_SUCCESS)
    {
        lastWinapiFunction = "RegCreateKeyA";
        throw winapiError;
    }
}

void openMessageBox()
{
    int msgbox = MessageBoxA(
        NULL,
        "MANAGMENT PROGRAM IS UP",
        "MANAGMENT PROGRAM",
        MB_OK
    );
    if (! msgbox)
    {
        lastWinapiFunction = "MessageBoxA";
        throw winapiError;
    }
}

int main()
{
    HANDLE mutex = ensureOneOrogram();
    
    
    try
    {
        RunOnStartUp();
        openMessageBox();
    }
    catch (WinapiError e)
    {
        if (e == winapiError)
        {
            lastError = GetLastError();
        }
        std::cout << "error number " << lastError << " in " << lastWinapiFunction <<"\n";    
    }
    if (mutex != NULL)
    {
        if (!ReleaseMutex(mutex))
        {
            std::cout << "error number " << GetLastError() << " in ReleaseMutex\n";    
        }
    }
    for (HANDLE h : handles)
    {
        CloseHandle(h);
    }

}

