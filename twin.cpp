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

#define DEFAULT_PORT "12345"

enum WinapiError
{
    winapiError,
    wsaError,
    otherError
};

const char* lastWinapiFunction = "";
std::vector<HANDLE> handles;
int lastError = 0;

BOOL wsaInitiated = FALSE;
PADDRINFOA addr = NULL;
std::vector<SOCKET> sockets;


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


void wsaInit()
{
    // init the wsa if needed
    if (!wsaInitiated)
    {
        WSADATA wsaData;
        int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0)
        {
            lastError = iResult;
            lastWinapiFunction = "WSAStartup";
            throw otherError;
        }
        wsaInitiated = TRUE;
    }
}

PADDRINFOA getServerAddr()
{
    // return a addrinfo of for the server

    struct addrinfo *result = NULL, *ptr = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the local address and port to be used by the server
    int iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
    {
        lastError = iResult;
        lastWinapiFunction = "getaddrinfo";
        throw otherError;
    }
    addr = result;
    return result;
}

SOCKET createSocket(PADDRINFOA result)
{
    // create a socket

    // Create a SOCKET for the server to listen for client connections
    SOCKET ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET)
    {
        lastWinapiFunction = "socket";
        throw wsaError;
    }
    sockets.push_back(ListenSocket);
    return ListenSocket;
}

void bindListenSocket(PADDRINFOA result, const SOCKET& ListenSocket)
{
    // bind a socket and listen (no blocking)

    // Setup the TCP listening socket
    int iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        lastWinapiFunction = "bind";
        throw wsaError;
    }

    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        lastWinapiFunction = "listen";
        throw wsaError;
    }
}

SOCKET acceptClient(const SOCKET& ListenSocket)
{
    SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET)
    {
        lastWinapiFunction = "accept";
        throw wsaError;
    }
    sockets.push_back(ClientSocket);
}

SOCKET waitForClient()
{
    wsaInit();

    PADDRINFOA result = getServerAddr();

    SOCKET ListenSocket = createSocket(result);

    bindListenSocket(result, ListenSocket);

    freeaddrinfo(addr);
    addr = NULL;


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
    HANDLE mutex = ensureOneOrogram();
    
    
    try
    {
        RunOnStartUp();
        openMessageBox();
    }
    catch (WinapiError e)
    {
        switch (e)
        {
        case winapiError:
            lastError = GetLastError();
            break;
        case wsaError:
            lastError = WSAGetLastError();
            break;
        default:
            break;
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

