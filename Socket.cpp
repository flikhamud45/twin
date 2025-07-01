#include "socket.h"

Socket::Socket(SOCKET s) : m_socket(s) {
    // blank intentionally
}

Socket::~Socket() { closesocket(m_socket); }

SOCKET Socket::getSocket() { return m_socket; }
BOOL wsaInitiated = FALSE;

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
    wsaInit();
    // return a addrinfo of for the server

    struct addrinfo *result = NULL, hints;

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
        throw WinapiException("socket", WinapiErrorType::wsaError);
    }
    return ListenSocket;
}

void bindSocket(PADDRINFOA result, const SOCKET& ListenSocket)
{
    // bind a socket
    int iResult = bind(ListenSocket, result->ai_addr, static_cast<int>(result->ai_addrlen));
    if (iResult == SOCKET_ERROR)
    {
        throw WinapiException("bind", WinapiErrorType::wsaError);
    }
}


void listenSocket(const SOCKET& ListenSocket)
{
    // listen on a socket (no blocking)

    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        throw WinapiException("listen", WinapiErrorType::wsaError);
    }
}


SOCKET acceptClient(const SOCKET& ListenSocket)
{
    SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET)
    {
        throw WinapiException("accept", WinapiErrorType::wsaError);
    }
    return ClientSocket;
    
}

ServerSocket::ServerSocket(const char* port) : m_addr(getServerAddr(port)), m_ListenSocket(createSocket(m_addr.getAddr())) {
    // blank
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

ClientSocket::ClientSocket(SOCKET s) : m_sock(s) {
    // blank
}

void ClientSocket::recvall(char* recvbuf, int recvbuflen) {
    int iResult = recv(m_sock.getSocket(), recvbuf, recvbuflen, MSG_WAITALL);
    if (iResult < recvbuflen || iResult == SOCKET_ERROR) {
        throw WinapiException("recv", WinapiErrorType::wsaError);
    }

}

ClientSocket::~ClientSocket() {
    // blank
}

auto _send = send;
void ClientSocket::send(const char* sendbuf, int len) {
    int iResult = _send(m_sock.getSocket(), sendbuf, len, 0);
    if (iResult == SOCKET_ERROR) {
        throw WinapiException("send", WinapiErrorType::wsaError);
    }
}

void ClientSocket::send(std::string s) {
    send(s.c_str(), static_cast<int>(s.size()));
}

std::string ClientSocket::recvMsg() {
    char sizeBuff[MSG_SIZE_SIZE];
    recvall(sizeBuff, MSG_SIZE_SIZE);
    int size = atoi(sizeBuff);
    char* msg = new char[size+1];
    recvall(msg, size);
    msg[size] = '\0';
    std::string ans = msg;
    delete[] msg;
    return ans;
}

void ClientSocket::sendMsg(const char* sendbuf, int len) {
    if (len > (1 << (MSG_SIZE_SIZE*8))) {
        throw std::exception("Invalid message size!");
    }
    char sizeBuff[MSG_SIZE_SIZE];
    send(sizeBuff, MSG_SIZE_SIZE);
    send(sendbuf, len);
}

void ClientSocket::sendMsg(std::string s) {
    sendMsg(s.c_str(), static_cast<int>(s.size()));
}






