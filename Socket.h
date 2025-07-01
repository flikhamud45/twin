#pragma once
#include "AddrInfo.h"
#define WIN32_LEAN_AND_MEAN

#include <Ws2tcpip.h>
#include <stdio.h>
#include <winsock2.h>

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")



constexpr int MSG_SIZE_SIZE = 2;

class Socket {
  public:
    Socket(SOCKET s);

    ~Socket();

    SOCKET getSocket();

  private:
    SOCKET m_socket;
};

class ClientSocket
{
public:
    ClientSocket(SOCKET s);

    ~ClientSocket();

    // recv recvbuflen bytes
    void recvall(char* recvbuf, int recvbuflen);

    // send len bytes from the given buffer
    void send(const char* sendbuf, int len);
    void send(std::string s);

    // recv a msg - recv size and than the actual message.
    std::string recvMsg();

    // send a msg - send size and than the actual message
    void sendMsg(const char* sendbuf, int len);
    void sendMsg(std::string s);


private:
    Socket m_sock;
    
};

class ServerSocket
{
public:
    ServerSocket(const char* port = DEFAULT_PORT);

    ~ServerSocket();

    void bind();

    void listen();

    ClientSocket accept();

private:
    Socket m_ListenSocket;
  AddrInfo m_addr;
};

