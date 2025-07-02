#pragma once
#include "Main.h"
#include "AddrInfo.h"

constexpr char DEFAULT_PORT[] = "12345";

// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")



constexpr int MSG_SIZE_SIZE = 2;
constexpr int FILE_SIZE_SIZE = 4;


class Socket {
  public:
    Socket();

    Socket(SOCKET s);

    Socket& operator=(Socket&& other) noexcept;

    ~Socket();

    SOCKET getSocket();

  private:
    SOCKET m_socket;
};

class ClientSocket {
  public:
    ClientSocket(SOCKET s);

    ~ClientSocket();

    // recv recvbuflen bytes
    void recvall(char* recvbuf, int recvbuflen);

    // send len bytes from the given buffer
    void sendall(const char* sendbuf, int len);
    void sendall(const std::string& s);

    // recv a msg - recv size and than the actual message.
    std::string recvMsg();

    // send a msg - send size and than the actual message
    void sendMsg(const char* sendbuf, int len);
    void sendMsg(const std::string& s);

    // recieve a file bye rceiving a msg of its name and than the size of the file and than the actual size
    void recvFile();

  private:
    Socket m_sock;
};

class ServerSocket {
  public:
    ServerSocket(const char* port = DEFAULT_PORT);

    ~ServerSocket();

    void bind();

    void listen();

    SOCKET accept();

  private:
    Socket m_ListenSocket;
    AddrInfo m_addr;
};

