#pragma once
#include "Closers.h"

class ClientSocket
{
public:
    ClientSocket(SOCKET s);

    ~ClientSocket();

    void recvall(const char* recvbuf, int recvbuflen);

    void send(const char* sendbuf, int len);

private:
    Socket sock;
    
};

class ServerSocket
{
    ServerSocket(const char* port);

    ~ServerSocket();

    void bind();

    void listen();

    ClientSocket accept();

private:
    Socket m_ListenSocket;
  AddrInfo m_addr;
};

