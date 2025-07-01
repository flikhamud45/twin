#pragma once
#define WIN32_LEAN_AND_MEAN

#include <Ws2tcpip.h>
#include <stdio.h>
#include <winsock2.h>


class AddrInfo {
  public:
    AddrInfo(PADDRINFOA addr);

    ~AddrInfo();

    PADDRINFOA getAddr();

  private:
    PADDRINFOA m_addr;
};
