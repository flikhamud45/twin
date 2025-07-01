
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "twin.h"

#include "handlers.h"
#include <iphlpapi.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>


#pragma comment(lib, "Ws2_32.lib")

constexpr char MUTEX_NAME[] = "technai_mutex";
constexpr char RUN_REG[] = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
constexpr WCHAR PROGRAM_NAME[] = L"Technai";
constexpr WCHAR PROGRAM_PATH[] = L"C:\\Users\\User\\source\\repos\\twin\\x64\\Debug\\twin.exe";



std::vector<SOCKET> sockets;



Mutex ensureOneOrogram()
{
    // ensure that there is only one program running and return a locked mutex.
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
    if (waitStatus == WAIT_OBJECT_0 || waitStatus == WAIT_ABANDONED)
    {
        return mutex;
    }
    else if (waitStatus == WAIT_TIMEOUT)
    {
        std::cout << "program is already running...\n";
        exit(1);
    }
    else if (waitStatus == waitStatus)
    {
        lastWinapiFunction = "WaitForSingleObject";
        throw WinapiError::winapiError;
    }
    else
    {
        std::cout << "Unknown error\n";
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
        throw WinapiError::otherError;
    }

    s = RegSetKeyValueW(
        hkey, 
        NULL,
        PROGRAM_NAME, 
        REG_SZ,
        PROGRAM_PATH, 
        static_cast<DWORD>(wcslen(PROGRAM_PATH)) + 1 // including the null terminator as needed according to the doc of the function
    );
    RegCloseKey(hkey);
    if (s != ERROR_SUCCESS)
    {
        lastWinapiFunction = "RegSetKeyValueW";
        throw WinapiError::winapiError;
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
        throw WinapiError::winapiError;
    }
}




SOCKET waitForClient()
{
    wsaInit();

    AddrInfo result = getServerAddr();

    SOCKET ListenSocket = createSocket(result.getAddr());

    bindListenSocket(result.getAddr(), ListenSocket);

    SOCKET ClientSocket = acceptClient(ListenSocket);

    closesocket(ListenSocket);
    ListenSocket = INVALID_SOCKET;
    return ClientSocket;
}


void talkWithClient(const SOCKET& ClientSocket)
{
    
}

void startServer()
{
    
    SOCKET ClientSocket = waitForClient();

    talkWithClient(ClientSocket);

}

int main()
{
    try
    {
        Mutex m = ensureOneOrogram();
        RunOnStartUp();
        openMessageBox();
    }
    catch (WinapiError e)
    {
        switch (e)
        {
        case WinapiError::winapiError:
            lastError = GetLastError();
            break;
        case WinapiError::wsaError:
            lastError = WSAGetLastError();
            break;
        default:
            break;
        }

        std::cout << "error number " << lastError << " in " << lastWinapiFunction <<"\n";    
    }
    catch (const std::exception& e)
    {
        std::cout << "Unknown exception: " << e.what() << "\n";
    }
    catch (...)
    {
        std::cout << "Unknown exception\n";
    }
    if (wsaInitiated)
    {
        WSACleanup();
    }
    if (addr)
    {
        freeaddrinfo(addr);
    }
    for (const SOCKET socket : sockets)
    {
        if (socket != INVALID_SOCKET)
        {
            closesocket(socket);
        }
    }

}

