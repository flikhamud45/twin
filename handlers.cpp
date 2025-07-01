#include "handlers.h"
#include <iostream>
#include <ws2tcpip.h>


Handle::Handle(HANDLE h) : m_handle(h)
{
    // blank intentionally
}

Handle::~Handle()
{
    CloseHandle(m_handle);
}

HANDLE Handle::getHandle()
{
    return m_handle;
}

Mutex::Mutex(HANDLE h) : Handle(h)
{
    // blank intentionally
}


Mutex::~Mutex()
{
    if (!ReleaseMutex(m_handle))
    {
        std::cout << "error number " << GetLastError() << " in ReleaseMutex\n";
    }
}

AddrInfo::AddrInfo(PADDRINFOA addr) : m_addr(addr)
{
    // blank intentionally
}

PADDRINFOA AddrInfo::getAddr()
{
    return m_addr;
}

AddrInfo::~AddrInfo()
{
    freeaddrinfo(m_addr);
}

Socket::Socket(SOCKET s) : m_socket(s)
{
    // blank intentionally
}

Socket::~Socket()
{
    closesocket(m_socket);
}

SOCKET Socket::getSocket()
{
    return m_socket;
}




