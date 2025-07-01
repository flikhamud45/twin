#include "socket.h"
#include "Main.h"
#include <windows.h>
#include <ws2tcpip.h>
#include <winsock2.h>

void wsaInit()
{
    // init the wsa if needed
    if (!wsaInitiated)
    {
        WSADATA wsaData;
        int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0)
        {
            throw WinapiException("WSAStartup", iResult);
        }
        wsaInitiated = TRUE;
    }
}

PADDRINFOA getServerAddr(const char* port)
{
    // return a addrinfo of for the server

    struct addrinfo *result = NULL, *ptr = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the local address and port to be used by the server
    int iResult = getaddrinfo(NULL, port, &hints, &result);
    if (iResult != 0)
    {
        throw WinapiException("getaddrinfo", iResult);
    }
    return result;
}

SOCKET createSocket(PADDRINFOA result)
{
    // create a socket

    // Create a SOCKET for the server to listen for client connections
    SOCKET ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET)
    {
        throw WinapiException("socket", WinapiError::wsaError);
    }
    return ListenSocket;
}

void bindSocket(PADDRINFOA result, const SOCKET& ListenSocket)
{
    // bind a socket
    int iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        throw WinapiException("bind", WinapiError::wsaError);
    }
}


void listenSocket(const SOCKET& ListenSocket)
{
    // listen on a socket (no blocking)

    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        throw WinapiException("listen", WinapiError::wsaError);
    }
}


SOCKET acceptClient(const SOCKET& ListenSocket)
{
    SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET)
    {
        throw WinapiException("accept", WinapiError::wsaError);
    }
    return ClientSocket;
    
}

ServerSocket::ServerSocket(const char* port)
{
    wsaInit();
    AddrInfo result = getServerAddr(port);
    m_addr = result;
    m_ListenSocket = createSocket(result.getAddr());

}

void ServerSocket::bind()
{
    bindSocket(m_addr.getAddr(), m_ListenSocket.getSocket());
}

void ServerSocket::listen()
{
    listenSocket(m_ListenSocket.getSocket());
}

ClientSocket ServerSocket::accept()
{
    return acceptClient(m_ListenSocket.getSocket());
}

ServerSocket::~ServerSocket()
{
    
}



