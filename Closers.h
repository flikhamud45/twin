#pragma once
#include <windows.h>
#include <ws2def.h>

class Handle
{
    // class that store a handle and handle destructor
  public:
    Handle(HANDLE h);

    ~Handle();

    HANDLE getHandle();

  protected:
    HANDLE m_handle;
};

class Mutex : Handle
{
    // class that store a mutex and handle destructor
  public:
    Mutex(HANDLE h);

    ~Mutex();
};


class AddrInfo
{
public:
    AddrInfo(PADDRINFOA addr);

    ~AddrInfo();

    PADDRINFOA getAddr();

private:
    PADDRINFOA m_addr;
};

class Socket
{
public:
    Socket(SOCKET s);

    ~Socket();

    SOCKET getSocket();

private:
    SOCKET m_socket;
};